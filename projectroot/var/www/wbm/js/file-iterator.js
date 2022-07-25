/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var FileIterator = (function (Xhr, options) {

    /**
     * @class FileIterator
     * @desc Create an file iterator object to iterate over a given file.
     * @example
     * var file = document.querySelector('input[type="file"]').files[0];
     * var iterator = new FileIterator(file, 1024);
     * while (iterator.hasNext()) {
     *     var blob = iterator.next();
     *     console.log('got a blob of size:', blob.size);
     * }
     * @param {File}   file      The file to iterate over
     * @param {number} chunkSize The chunk size
     */
    function FileIterator (file, chunkSize) {
        this.file = file;
        this.offset = 0;
        this.chunkSize = chunkSize;
    }

    /**
     * @method FileIterator#next
     * @desc Get the next data chunk with the given length of bytes or 
     * less, if less is available
     * @param {number} count The number of bytes to request
     * @returns {Blob|null} the slice of a file as a blob object or null, 
     *                      if no data is left
     */
    FileIterator.prototype.next = function () {
        var blob = this.file.slice(this.offset, this.offset + this.chunkSize);
        this.offset += blob.size;
        if (blob.size == 0) {
            return null;
        }
        return blob;
    }

    /**
     * @method FileIterator#position
     * @desc Get the next position of the next chunk
     * @returns {number} The position as integer
     */
    FileIterator.prototype.position = function () {
        return this.offset;
    }

    /**
     * @method FileIterator#hasNext
     * @desc Retrieve information wether any more data is availablie via this.next() or not.
     * @returns {boolean} true, if this.next() woule return any data at this 
     *                    moment. false otherwise.
     */
    FileIterator.prototype.hasNext = function () {
        return this.offset < this.file.size;
    }
    
    return FileIterator;
})();