<?php namespace transfer;

/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

include_once 'fs_utils/fs_utils.php';
include_once 'ini_utils/ini_utils.php';

use function ini_utils\max_post_size;
use function fs_utils\rmkdir;
use function fs_utils\rrmdir;
use function fs_utils\dirsize;

class FileTransfer {


    /**
     * The indentifier used to identify a transfer. 
     */
    private $token = null;
    
    /**
     * The directory where the file transfer stores its files
     */
    private $path = null;

    /**
     * The directory where the file transfers $path is located in
     */
    private $basepath = null;

    /**
     * The size left for this transfer. if anything uses this transfer it should call the 
     * take($bytes) method to tell the transfer that some of its size is used. -1: no limit.
     */
    private $size = -1;

    /**
     * true, if this transfer is broken, which may occur, if a cleanup 
     * has failed.
     */
    private $broken = false;

    
    /**
     * true, if the transfer required to mount the extra partition
     */
    private $mounted = false;

    /** 
     * Create a file transfer object either by reference, when a token 
     * is given, or a new one, if none is given. 
     */
    private function __construct() { }



    /**
    * Creates a new file transfer by loading information about an existing 
    * transfer from an internal cache using the given token.
    * 
    * @param string token The token to be used to identify the to-be-loaded 
    *               transfer information
    *
    * @return FileTransfer|null
    */
    public static function reopen($token) {

        // no cached transfer with this token
        if (!isset($_SESSION['file_transfer']) || !isset($_SESSION['file_transfer'][$token])) 
            return null;

        $transfer = new FileTransfer();
        $transfer->token = $token;
        $transfer->loadFromCache();
        
        return $transfer;
    }



    /**
    * Creates a new file transfer. In dependency of the size 
    * of the transfer a different setup may apply. 
    *
    * @param int size the overall size of the transfer
    *
    * @return FileTransfer|null
    */
    public static function create($size) {

        $transfer = new FileTransfer();

        if ($size == 0) {
            $size = -1; // unlimited
        }

        $transfer->size = $size;

        // setup cache object if not present
        if (!isset($_SESSION['file_transfer']))
            $_SESSION['file_transfer'] = [];

        // generate some unique token using the current timstamp
        do {
            $token = \str_pad((string)microtime(true), 16, '0');
        } while (isset($_SESSION['file_transfer'][$token]));
        $transfer->token = $token;
        
        $basepath;
        if ($size == -1 || $size > max_post_size()) {
            if ($basepath = $transfer->mount()) {
                $basepath = $basepath . '/transfer';
                $transfer->mounted = true;
            } else {
                // ERROR mount failed (not enough memory) 
                return null;
            }
        } else {
            $basepath = '/tmp/transfer';
        }
        $transfer->basepath = $basepath;
        $transfer->path = "$basepath/$token";
        

        // create the directory. If that fails, the transfer is not created
        if (!rmkdir($transfer->path)) {
            return null;
        }

        $transfer->saveToCache();
        return $transfer;
    }



    
    private function mount() {
        $path = null;

        // check if partition is mounted by ommiting the -p (--prepare) argument               
        $result = exec('sudo /etc/config-tools/file_transfer -q -d -j -a', $_, $status);       
        $result = json_decode($result, true);                                                  
                                                                                               
        if ($status != 0 || $result['FileTransferPath'] == 'none' ) {                          
            // try to mount, if failed or not mounted yet                                      
            $result = exec('sudo /etc/config-tools/file_transfer -q -p -d -j -a', $_, $status);
            $result = json_decode($result, true);                                              
        }                                                                                      
                                                                                               
        if ($status == 0) {                                                       
                                                                                  
            if ($this->size != -1 && $result['AvailableCapacity'] < $this->size) {
                $this->unmount();                                                 
                $path = null;                                                     
            } else {
                $path = $result['FileTransferPath'];
                
                // increment mount counter
                $file = fopen("$path/TRANSFER", 'c+');
                flock($file, LOCK_EX);
                $count = intval(fread($file, filesize("$path/TRANSFER")));
                $count += 1;
                ftruncate($file, 0);
                rewind($file);
                fwrite($file, "$count");
                flock($file, LOCK_UN);
                fclose($file);
            }
        }
        return $path;
    }

    private function unmount() {
        $path = dirname($this->basepath);

        // open counter
        $file = fopen("$path/TRANSFER", 'c+');
        flock($file, LOCK_EX);
        $count = intval(fread($file, filesize("$path/TRANSFER")));
        $count -= 1;

        if ($count > 0) {
            // don't unmount, but decrement a counter
            ftruncate($file, 0);
            rewind($file);
            fwrite($file, "$count");
            flock($file, LOCK_UN);
            fclose($file);
            return true;

        } else {
            flock($file, LOCK_UN);
            fclose($file);

            // unmount
            $result = exec('sudo /etc/config-tools/file_transfer -q -c', $_, $status);
            if ($status == 0) {
                return true;
            } else {
                return false; // ERROR: should have been unmounted, but failed
            }
        }
    }

    private function loadFromCache() {
        $cache = $_SESSION['file_transfer'][$this->token];
        $this->path = $cache['path'];
        $this->basepath = $cache['basepath'];
        $this->broken = $cache['broken'];
        $this->size = $cache['size'];
        $this->mounted = $cache['mounted'];
    }

    private function saveToCache() {
        $_SESSION['file_transfer'][$this->token] = [
            'path' => $this->path,
            'basepath' => $this->basepath,
            'size' => $this->size,
            'broken' => $this->broken,
            'mounted' => $this->mounted
        ];
    }


    public function takeBytes($count) {
        if ($this->size == -1) {
            // take from infinite size
            return true;

        } else if ($this->availableBytes() < $bytes) { 
            return false;
        } else {
            $this->size -= $count;
            $this->saveToCache();
            return true;
        }
    }


    /**
     * Get the amount of bytes available to be stored in this transfers
     * context. If the value is negative, more bytes are stored, than reserved. 
     * However, this might not be a problem, as no prevention is applied to 
     * exceed the transfer size. 
     *
     * @return int number of bytes available 
     */
    public function availableBytes() {
        
        if ($this->size == -1) { return INF; }

        return $this->size - dirsize($this->path);
    }

    /**
     * Get the transfer path
     */
    public function path() {
        return $this->path;
    }

    /**
     * Generate a file path pointing to a file with a given name inside 
     * the base path of this transfer.
     */
    public function filepath($filename) {
        return "$this->path/$filename";
    }


    /**
     * Get the transfer token
     */
     public function token() {
        return $this->token;
    }


    /**
    * Perform cleanup steps if a transfer is not beeing in use any 
    * more. This always includes a removal of any files and directories
    * associated with the transfer. Also, if the cleanup succeeds,
    * the transfer will be removed from an internal cache, which will 
    * cause it to not beeing beeing re-openable any more. If the cleanup 
    * fails, the transfer is marked as beeing corrupted, but is still 
    * reopenable from cache.
    *
    * @return boolean true, if cleanup was successful, false otherwise
    */
    public function cleanup() {

        // clear whole path
        rrmdir($this->path);

        if ($this->mounted) {
            $this->unmount();
        }

        // remove self from internal cache.
        unset($_SESSION['file_transfer'][$this->token]);
    }

}