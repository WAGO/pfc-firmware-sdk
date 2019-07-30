<?php namespace transfer;

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

include_once 'transfer/file_transfer.php';
include_once 'fs_utils/fs_utils.php';

use function fs_utils\rmkdir;

class FileUpload {

    /**
     * The file handle to append received data to
     */
    private $file = false;

    /**
     * The filename
     */
    private $filename = null;

    /**
     * The expected filesize
     */
    private $filesize = 0;

    /**
     * The destination filename
     */
    private $destination = null;

    /**
     * The transfer beeing used to store the upload
     */
    private $transfer = null;

    /**
     * Create new upload 
     * @param FileTransfer transfer A transfer to be used to store the uploaded file
     * @param string       filename The filename to be used for the file
     * @param int          filesize The expected filesize. The upload is considered 
     *                     as completed, if this filesize is reached.
     * @param string       detination The destination filename
     */
    private function __construct($token, $transfer, $filename, $filesize, $destination = null) {
        $this->token = $token;
        $this->transfer = $transfer;
        $this->filename = $filename;
        $this->filesize = $filesize;
        $this->destination = $destination;

        $filepath = $transfer->filepath("$filename.incomplete");
        if (file_exists($filepath)) {
            $this->file = fopen($filepath, 'ab');
        }
    }


    /**
     * Resume a running upload
     * @param string token The token to identify the upload
     * @return FileUpload|null 
     */
    public static function resume($token) {

        // not found in cache
        if (!isset($_SESSION['uploads']) || !isset($_SESSION['uploads'][$token])) 
            return null;

        $cache = $_SESSION['uploads'][$token];

        $transfer = FileTransfer::reopen($cache['transfer']);
        $filename = $cache['filename'];
        $filesize = $cache['filesize'];
        $destination = $cache['destination'];
        
        return new FileUpload($token, $transfer, $filename, $filesize, $destination);
    }


    /**
     * Create a new file upload object
     * @param FileTransfer transfer A transfer to be used to store the uploaded file
     * @param string       filename The filename to be used for the file
     * @param int          filesize The expected filesize. The upload is considered 
     *                     as completed, if this filesize is reached.
     * @param string       detination The destination filename
     * @return FileUpload|null 
     */
    public static function create($transfer, $filename, $filesize, $destination = null) {

        // create cache object
        if (!isset($_SESSION['uploads'])) 
            $_SESSION['uploads'] = [];

        // check if filesize is available
        if (false == $transfer->takeBytes($filesize)) {
            return null;
        }

        $token;
        do {
            $token = \dechex(\mt_rand());
        } while (!$token || $_SESSION['uploads'][$token]);

        // create file and upload object
        touch($transfer->filepath("$filename.incomplete"));
        $upload = new FileUpload($token, $transfer, $filename, $filesize, $destination);

        $_SESSION['uploads'][$token] = [
            'transfer' => $transfer->token(),
            'filename' => $filename,
            'filesize' => $filesize,
            'destination' => $destination
        ];

        return $upload;
    }

    /**
     * Get the upload token
     */
     public function token() {
        return $this->token;
    }

    /**
     * Store a chunk in the file 
     */
    public function process($bytes) {
        if ($this->file == false) {
            return false;
        }
        // TODO: check if bytes are available (?)
        return fwrite($this->file, $bytes, strlen($bytes));
    }

    /**
     * Check for upload completion (filesize reached). If an upload has been completed, it
     * cannot be processed anymore. However, the file handle of the upload is closed anyway, 
     * when calling this method. You'll need to resume the upload to be able to process any
     * more data.
     *
     * @return string|false filepath of completed upload or false
     */
    public function finalize() {
        
        // filepath of potentially completed upload
        $filepath = $this->transfer->filepath($this->filename);
        $destination; 
        if ($this->destination == null) {
            $destination = $filepath;
        } else if ($this->destination[0] == '/') {
            $destination = $this->destination;
        } else {
            $destination = $this->transfer->filepath($this->destination);
        }
        

        // if finalize() is called a second time 
        if (file_exists($destination) && !file_exists("$filepath.incomplete")) {
            return $destination;
        } 

        // if called without a valid file member
        else if ($this->file == false) {
            return false;
        }

        // close file and forget handle
        fclose($this->file);
        $this->file = false;

        // check for completion
        if (filesize("$filepath.incomplete") >= $this->filesize) {

            // move uploaded file to its destination directory
            // which _may_ be outside of the transfers' scope,
            // if a specific destination was given on upload 
            // creation. 
            
            // If the file exists, remove it first
            if (file_exists($destination)) {
                unlink($destination);
            }

            // create destiation directory on demand
            rmkdir(dirname($destination));

            // move uploaded file to its destination
            rename("$filepath.incomplete", $destination);

            // cleanup cache -> upload will not be available any more
            unset($_SESSION['uploads'][$token]);

            return $destination;
        } 

        return false;
    }

}