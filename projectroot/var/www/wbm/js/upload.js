/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var Upload = (function(Xhr, FileIterator, FileTransfer, options) {
    
    /**
     * @typedef {Object} FileObject
     * @property {File}   file          The actual File object to be uploaded
     * @property {string} [destination] Can either be an absolute or relative filepath.
     *                                  If omitted or relative, the file will be stored 
     *                                  in a temporary directory, which will be removed 
     *                                  on cleanup (calling upload.cleanup()). If a 
     *                                  destination is given, the file will be renamed 
     *                                  and moved to the given path.
     *                                  If an absolute path is provided, enough space to
     *                                  store the given file must be available there!
     */

    /**
     * @constructor Upload
     * @desc Create a new upload object, which may be used to upload one or more files
     * @example
     * var input1 = document.querySelector('#fileinput1');
     * var input2 = document.querySelector('#fileinput2');
     * var files = [
     *     input1.files[0], 
     *     {
     *         file: input2.files[0], 
     *         destination: '/tmp/blah/blub/' + input2.files[0].name 
     *     }
     * ];
     * 
     * var upload = new Upload(files);
     * 
     * upload.execute(function(filepaths, error) {
     *     if (error) {
     *         // handle error
     *     } else {
     *        console.log(filepaths);
     *        // filepaths[2] should be the given destination ;)
     *     }
     *     upload.cleanup(function(error) {
     *         if (error) {
     *             //cleanup failed!
     *         }
     *     })
     * })
     * @param {File|FileObject|Array<File|FileObject>} files Files to be uploaded
     */
    function Upload(files) {
        
        // this.files must be an array of FileObject objects
        if (files instanceof Array || files instanceof FileList) {
            this.files = [];
            for (var i = 0; i < files.length; i++) {
                this.files.push(files[i]);
            }
        } else {
            this.files = [files];
        }
        for (var i = 0; i < this.files.length; i++) {
            if (this.files[i] instanceof File) {
                this.files[i] = {
                    file: this.files[i]
                }
            }
        }

        this.transfer = null;
    }


    /**
     * @private
     * Upload the given file recursively
     * @param {string}       transferToken
     * @param {string}       uploadToken
     * @param {FileIterator} iterator
     * @param {string}       callback
     */
    function uploadChunkwise(transferToken, uploadToken, iterator, callback) {

        var xhr = new Xhr('POST', options.uploadUrl);
        xhr.customHeader('Transfer-Token', transferToken);
        xhr.customHeader('Upload-Token', uploadToken);
        
        if (!iterator.hasNext()) {
            callback(null, new Error('No data left, but upload still incomplete'));
            return;
        }

        xhr.send(iterator.next(), function (response, error) {
            if (error) callback(null, error);
            else if (response.status != 0)  callback(null, new Error(response.statusMessage));
            else {
                if (response.uploadPath) {
                    callback(response.uploadPath);
                } else if (response.uploadToken) {
                    uploadChunkwise(transferToken, response.uploadToken, iterator, callback);
                } else {
                    callback(null, new Error('Invalid Response.'));
                }
            }
        });
    }

    /**
     * @private
     * Upload the given file 
     * @param {string}       transferToken
     * @param {number}       chunkSize
     * @param {FileObject}   fileObject
     * @param {Function}     callback
     */
    function uploadSingleFile(transferToken, chunkSize, fileObject, callback) {
        var xhr = new Xhr('POST', options.uploadUrl);
        xhr.customHeader('Transfer-Token', transferToken);
        xhr.customHeader('Upload-Name', encodeURIComponent(fileObject.file.name));
        xhr.customHeader('Upload-Size', fileObject.file.size);
        if (fileObject.destination) {
          xhr.customHeader('Upload-Destination', encodeURIComponent(fileObject.destination));
        }
        var iterator = new FileIterator(fileObject.file, chunkSize);

        if (!iterator.hasNext()) {
            callback(null, new Error('No data to upload'));
            return;
        }

        xhr.send(iterator.next(), function(response, error) {
            if (error) callback(null, error);
            else if (response.status != 0)  callback(null, new Error(response.statusMessage));
            else {
                if (response.uploadPath) {
                    callback(response.uploadPath);
                } else if (response.uploadToken) {
                    uploadChunkwise(transferToken, response.uploadToken, iterator, callback);
                } else {
                    callback(null, new Error('Invalid Response'));
                }
            }
        })
    }


    /**
     * @private
     * Upload the given files recursively
     * @param {FileObject[]} files
     * @param {Function}     callback
     */
    function uploadFiles(transferToken, chunkSize, files, callback) {
        var next = files.shift();

        if (!next) {
            callback(null, new Error('No Files to upload.'));
            return;
        }

        uploadSingleFile(transferToken, chunkSize, next, function(uploadPath, error) {
            if (error) callback(null, error);
            else {
                if (files.length == 0) {
                    callback([uploadPath]);
                } else {
                    uploadFiles(transferToken, chunkSize, files, function(uploadPaths, error) {
                        if (error) callback(null, error);
                        else {
                            uploadPaths.unshift(uploadPath);
                            callback(uploadPaths);
                        }
                    });
                }
            }
        });
    }


    /**
     * @callback Upload.ExecuteCallback
     * @desc Called when the upload is done or an error occured
     * @param {Array<string>} [filepaths] The filepaths were the individual files were stored on a device.
     *                                    These are either temporary directory paths or the originally given
     *                                    destination paths of the uploaded files.
     * @param {Error}         [error]     Will contain an an error object as far as an error occured.
     */

    /**
     * @method Upload#execute
     * @desc Perform the uploads
     * @param {ExecuteCallback} [callback] Called, when all uploads are done.
     */
    Upload.prototype.execute = function(callback) {
        var upload = this;

        if (!callback) callback = function(_, error) { 
            error && console.error(error); 
        }

        var files = [];
        var size = 0;
        for(var i = 0; i < this.files.length; i++) {
            size += this.files[i].file.size;
            files.push(this.files[i]);
        }

        this.transfer = new FileTransfer();
        this.transfer.prepare(size, function(error) {
            if (error) callback(null, error);
            else {
                uploadFiles(upload.transfer.transferToken, upload.transfer.uploadChunkSize, files, function (result, error) {
                    if (error) {
                        upload.cleanup();
                    }
                    callback(result, error);
                });
            }
        })
    }

    /**
     * @callback Upload.CleanupCallback
     * @desc Called when the upload cleanup is done or an error occured
     * @param {Error} [error] Will contain an an error object as far as an error occured.
     */

    /**
     * @method Upload#cleanup
     * @desc Perform a cleanup. Files which were not moved on the device by using the 
     * destination field of their FileObject, may not be available on the device 
     * after calling this method.
     * @param {CleanupCallback} [callback] Called, when cleanup is done.
     */
    Upload.prototype.cleanup = function(callback) {
        if (!this.transfer) callback(new Error('No Transfer to cleanup'));
        else {
            callback = callback || function(error) { error && console.error(error); }
            this.transfer.cleanup(callback);
        }
    }




    return Upload;
})(
    Xhr, 
    FileIterator, 
    FileTransfer, {
        uploadUrl: '/wbm/receive_upload.php'
    }
);