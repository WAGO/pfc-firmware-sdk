/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var Xhr = (function() {
    
    /**
     * @class Xhr
     * @desc Create a new XML HTTP request using the given method and URL.
     * @example
     * var xhr = new Xhr('POST', '/abc/123');
     * xhr.send({some: 'data'}, function(response, error) {
     *     if (error) {
     *         // handle error
     *     } else {
     *         // handle response
     *         console.log('response received:', response);
     *     }
     * })
     * @param {string} method Any HTTP method
     * @param {string} url    URL for the request
     */
    function Xhr(method, url) {
        this.xhr = new XMLHttpRequest();
        this.xhr.open(method, url);
    }

    /**
     * @static 
     * @desc Set session token to be used among all requests
     */
    Xhr.sessionToken = null;

    /**
     * @method Xhr#header
     * @desc Set any header field of the request
     * @param {string} name  The name of the header to set a value for
     * @param {string} value The value for the given header field
     */
    Xhr.prototype.header = function(name, value) {
        this.xhr.setRequestHeader(name, value);
    }

    /**
     * @method Xhr#customHeader
     * @desc Shorthand for calling `this.header('X-Wbm-<name>', value)`
     * @param {string} name  The name of the header to set a value for
     * @param {string} value The value for the given header field
     */
    Xhr.prototype.customHeader = function(name, value) {
        this.xhr.setRequestHeader('Com-Wago-' + name, value);
    }

    /**
     * @callback Xhr.SendCallback
     * @desc Called when the preparation step of an upload has been completed
     * @param {JsonSerializable} [response] The response as returned by the server.
     * @param {Error}            [error]    Will contain an an error object as far as an error occured.
     */

    /**
     * @method Xhr#send
     * @desc Send any JSON data or File object to the requests URL.
     * @param {JsonSerializable|File} [data]       JSON object or primitive or file to be sent.
     * @param {SendCallback}          [callback] Called when the request has been completed and a reponse 
     *                                           from the server was received.
     */
    Xhr.prototype.send = function(data, callback) {
        if (typeof data == 'function') { callback = data; }

        var xhr = this.xhr;
        this.xhr.onreadystatechange = function(readyState) {
            switch (xhr.readyState) {
                case 4: {
                    if (xhr.status == 200) {
                        if (xhr.response != null) {
                            callback && callback(xhr.response ? JSON.parse(xhr.response) : undefined);
                        }
                    } else {
                        callback && callback(null, new Error('Received Server Status:', xhr.status, xhr.statusText));
                    }
                }
            }
        };
        this.customHeader('Session-Token', Xhr.sessionToken); 
        if (data instanceof File || data instanceof Blob) {
            this.xhr.send(data);
        } else {
            this.xhr.send(JSON.stringify(data));
        }
    }

    return Xhr;
})();