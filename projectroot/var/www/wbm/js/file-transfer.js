/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var FileTransfer = (function(Xhr, options) {

    /**
     * @constructor FileTransfer
     * @desc Create a new file transder object, which may be used to upload or download one or more files
     */
    function FileTransfer() {
        this.uploadChunkSize = null;
        this.transferToken = null;
        this.transferPath = null;
    }

    /**
     * @method FileTransfer#prepare
     * @desc Call to prepare a transfer
     * @param {number}   [size]   The size of the transfer in bytes. Omitting this value 
     *                            disables any size checks, but should be done, if the size 
     *                            is unknown.
     * @param {Function} callback Called when preparation is done or has failed
     */
    FileTransfer.prototype.prepare = function(size, callback) {
        var transfer = this;

        if (typeof (size) == 'function') {
            callback = size;
            size = undefined;   
        }

        var xhr = new Xhr('POST', options.prepareTransferUrl);

        if (size != undefined) {
            xhr.customHeader('Transfer-Size', size);
        }

        xhr.send(undefined, function(response, error) {
            if (error) callback(null, error);
            else if (response.status != 0) callback(new Error(response.statusMessage));
            else {
                transfer.uploadChunkSize = response.uploadChunkSize;
                transfer.transferToken = response.transferToken;
                transfer.transferPath = response.transferPath;
                callback();
            }
        });
    }


    /**
     * @callback FileTransfer.CleanupCallback
     * @desc Called when the cleanup is done or an error occured
     * @param {Error} [error] Will contain an an error object as far as an error occured.
     */

    /**
     * @method FileTransfer#cleanup
     * @desc Perform a cleanup. This will remove all files associated with this transfer
     * @param {CleanupCallback} [callback] Called, when cleanup is done.
     */
    FileTransfer.prototype.cleanup = function(callback) {
        if (!this.transferToken) callback(/*nothing to to*/);
        else {
            callback = callback || function(error) { error && console.error(error); }

            var xhr = new Xhr('POST', options.cleanupTransferUrl);
            xhr.customHeader('Transfer-Token', this.transferToken);
            xhr.send(undefined, function(response, error) {
                if (error) callback(error);
                else if (response.status != 0) callback(new Error(response.statusMessage));
                else {
                    callback();
                }
            });
        }
    }




    return FileTransfer;
})(
    Xhr, {
        prepareTransferUrl: '/wbm/prepare_transfer.php',
        cleanupTransferUrl: '/wbm/cleanup_transfer.php'
    }
);