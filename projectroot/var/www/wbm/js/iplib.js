
/*
 * +--------------------------------------------------------------------------+
 * | IPlib.js                                                                 |
 * +--------------------------------------------------------------------------+
 * | Copyright (c) 2012 Endian                                                |
 * |         Endian GmbH/Srl                                                  |
 * |         Bergweg 41 Via Monte                                             |
 * |         39057 Eppan/Appiano                                              |
 * |         ITALIEN/ITALIA                                                   |
 * |         info@endian.com                                                  |
 * |                                                                          |
 * | IPlib.js is free software: you can redistribute it and/or modify         |
 * | it under the terms of the GNU Lesser General Public License as published |
 * | by the Free Software Foundation, either version 2.1 of the License, or   |
 * | (at your option) any later version.                                      |
 * |                                                                          |
 * | IPlib.js is distributed in the hope that it will be useful,              |
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of           |
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
 * | GNU Lesser General Public License for more details.                      |
 * |                                                                          |
 * | You should have received a copy of the GNU Lesser General Public License |
 * | along with IPlib.js.  If not, see <http://www.gnu.org/licenses/>.        |
 * +--------------------------------------------------------------------------+
 *
 * IPlib.js is a JavaScript library very inspired by the IPlib python module
 * useful to convert amongst many different notations and to manage couples
 * of address/netmask in the CIDR notation.
 * Thanks to Davide Alberani for the original IPlib.
 *
 * @author Andrea Bonomi <a.bonomi@endian.com>
 * @version 1.0
 */

(function(window,undefined){

  // Use 'jQuery' as the namespace only if it already exists
  var $ = window.jQuery || window;

  // Notation types
  //$.IP_UNKNOWN = 0;
  $.IP_DOT  = 1; // 192.168.0.42
  //IP_HEX  = 2; // 0xC0A8002A
  $.IP_BIN  = 3; // 11000000101010000000000000101010
  //IP_OCT  = 4; // 030052000052
  $.IP_DEC  = 5; // 3232235562
  $.IP_BITS = 6; // 26
  
  // This dictionary maps NM_BITS to NM_DEC values
  VALID_NETMASKS = {  0:  0,
                      1:  2147483648,  2: 3221225472,  3: 3758096384,
                      4:  4026531840,  5: 4160749568,  6: 4227858432,
                      7:  4261412864,  8: 4278190080,  9: 4286578688,
                      10: 4290772992, 11: 4292870144, 12: 4293918720,
                      13: 4294443008, 14: 4294705152, 15: 4294836224,
                      16: 4294901760, 17: 4294934528, 18: 4294950912,
                      19: 4294959104, 20: 4294963200, 21: 4294965248,
                      22: 4294966272, 23: 4294966784, 24: 4294967040,
                      25: 4294967168, 26: 4294967232, 27: 4294967264,
                      28: 4294967280, 29: 4294967288, 30: 4294967292,
                      31: 4294967294, 32: 4294967295 }
  
  var NETMASKS_INV = {};
  for (var i=0; i<=32; i++) {
      NETMASKS_INV[VALID_NETMASKS[i]] = i;
  }
  
  function isDot(ip) {
      /* Return true if the IP address is in dotted decimal notation */
      try {    
          var octets = ip.split(/\./g);
          if (octets.length != 4)
              return false;
          for (var i=0; i<octets.length; i++) {
              val = parseInt(octets[i]);
              if (isNaN(val) || val < 0 || val > 255)
                  return false;
          }
          return true;
      } catch (err) {
          return false;
      }
  }
  
  function dotToDec(ip, check) {
      /* Dotted decimal notation to decimal conversion */
      if ((check || check == undefined) && !isDot(ip))
          throw "invalid IP: " + ip;
      var octets = ip.split(/\./g);
      var dec = 0;
      dec |= parseInt(octets[0]) << 24;
      dec |= parseInt(octets[1]) << 16;
      dec |= parseInt(octets[2]) << 8;
      dec |= parseInt(octets[3]);
      return dec >>> 0;
  }
  
  function decToDot(ip) {
      /* Decimal to dotted decimal notation conversion */
      var first = (ip >>> 24)  & 255;
      var second = (ip >>> 16) & 255;
      var third = (ip >>> 8) & 255;
      var fourth = ip & 255;
      return first + "." + second + "." + third + "." + fourth;
  }
  
  function isBitsNm(nm) {
      /* Return true if the netmask is in bits notatation */
      var bits = parseInt(nm);
      if (isNaN(bits) || bits < 0 || bits > 32)
          return false;
      return true;
  }
  
  function bitsToDec(nm, check) {
      /* Bits to decimal conversion */
      if ((check || check == undefined) && !isBitsNm(nm))
          throw "invalid netmask: " + nm;
      var bits = parseInt(nm);
      return VALID_NETMASKS[bits];
  }
  
  function decToBits(nm) {
      /* Decimal to bits conversion. */
      return NETMASKS_INV["" + nm];
  }

  function isBin(ip) {
      /* Return true if the address is in binary notatation */
      var result = String(ip);
      if (result.length != 32)
          return false;
      for (var i=0; i<ip.length; i++)
          if (result[i] != "0" && result[i] != "1")
              return false;
      return true;
  }
  
  function decToBin(ip) {
      /* Decimal to binary conversion. */
      var result = "";
      ip = String(ip);
      for(var i = 0x80000000; i >= 1; i /= 2)
          result += (ip & i) ? "1" : "0";
      return result;
  }

  function binToDec(ip, check) {
      /* Binary to decimal conversion. */
      if ((check || check == undefined) && !isBin(ip))
          throw "invalid: " + ip;
      result = 0;
      for (var i=0; i<ip.length; i++) {
         result = ((result << 1) + ((ip[i] == "1") ? 1 : 0)) >>> 0;
      }
      return result;
  }
  
  function isDec(ip) {
      /* Return true if the address is decimal notatation */
      var result = parseInt(ip);
      if (isNaN(result))
          return false;
      result = result >>> 0;
      if (result < 0 || result > 0x100000000)
          return false;
      return true;
  }
  
  function decToDec(ip, check) {
      /* Decimal to decimal conversion. */
      if ((check || check == undefined) && !isDec(ip))
          throw "invalid: x" + ip;
      return parseInt(ip);
  }

  $.IPv4Address = function(ip, notation) {
      /*
       * An IPv4 Internet address.
       * This class represents an IPv4 Internet address.
       */
      this.set = function(ip, notation) {
          if (notation == $.IP_DEC) {
              this.ipDec = decToDec(ip);
          } else if (notation == $.IP_BIN) {
              this.ipDec = binToDec(ip);
          } else if (notation == $.IP_DOT) {
              this.ipDec = dotToDec(ip);
          } else {
              // guess the notation
              if (isBin(ip)) {
                  this.ipDec = binToDec(ip);
              } else if (isDot(ip)) {
                  this.ipDec = dotToDec(ip);
              } else {
                  this.ipDec = decToDec(ip);
              }
          }
          this.ip = decToDot(this.ipDec);
      };
      this.getDec = function() {
          /* Return the decimal notation of the address */
          return this.ipDec;
      };
      this.getDot = function() {
          /* Return the dotted decimal notation of the address */
          return this.ip;
      };
      this.getBin = function() {
          /* Return the binary notation of the address */
          return decToBin(this.ipDec);
      };
      this.toString = function() {
          return this.ip;
      };
      this.equals = function(other) {
          /* Compare this IPv4Address object with another object */
          return String(this) == String(other);
      };
      this.set(ip, notation);
  }
  
  $.IPv4NetMask = function(nm, notation) {
      /*
       * An IPv4 Internet netmask.
       * This class represents an IPv4 Internet netmask.
       */
      this.set = function(nm, notation) {
          if (notation == $.IP_DEC) {
              nm = nm >>> 0;
              if (nm < 0 || nm > 0x100000000)
                  throw "invalid: " + nm; 
              this.nmDec = nm;
          } else if (notation == $.IP_BIN) {
              this.nmDec = binToDec(nm);
          } else if (notation == $.IP_BITS) {
              this.nmDec = bitsToDec(nm);
          } else {
	      // guess the notation
	      if (isBin(nm)) {
                  this.nmDec = binToDec(nm);
              } else if (isDot(nm)) {
                  this.nmDec = dotToDec(nm);
              } else {
                  this.nmDec = bitsToDec(nm);
              }
          };
          this.nm = decToBits(this.nmDec);
      };
      this.getDec = function() {
          /* Return the decimal notation of the netmask */
          return this.nmDec;
      };
      this.getDot = function() {
          /* Return the dotted decimal notation of the netmask */
          return decToDot(this.nmDec);
      };
      this.getBin = function() {
          /* Return the binary notation of the netmask */
          return decToBin(this.nmDec);
      }
      this.getBits = function() {
          /* Return the bits notation of the netmask */ 
          return this.nm;
      };
      this.toString = function() {
          return this.nm;
      };
      this.equals = function(other) {
          /* Compare this IPv4Netmask object with another object */
          return String(this) == String(other);
      };
      this.set(nm, notation);
  }
  
  $.CIDR = function(cidr) {
      this.set = function(cidr) {
          var s;
          try {
              s = cidr.split("/", 2);
          } catch (err) {
              throw "invalid CIDR: " + cidr;
          }
          if (s.length != 2)
              throw "invalid CIDR: " + cidr;
          this.ip = new $.IPv4Address(s[0]);
          this.nm = new $.IPv4NetMask(s[1]);
          var ipl = this.ip.ipDec;
          var nml = this.nm.nmDec;
          this.netIpDec = (ipl & nml) >>> 0;
          this.netIp = new $.IPv4Address(this.netIpDec, $.IP_DEC);
          this.ipNum = 0xFFFFFFFF - 1 - nml;
          // This's here to handle /32 (-1) and /31 (0) netmasks
          if (this.ipNum == -1 || this.ipNum == 0) {
              if (this.ipNum == -1) {
                  this.ipNum = 1;
              } else {
                  this.ipNum = 2;
              }
              // broadcast address
              this.bcIpDec = null;
              this.bcIp = null;
              // first ip
              this.firstIpDec = this.netIpDec;
              this.firstIp = new $.IPv4Address(this.firstIpDec, $.IP_DEC);
              // last ip
              if (this.ipNum == 1) {
                  this.lastIpDec = this.firstIpDec;
                  this.lastIp = this.firstIp;
              } else {
                  this.lastIpDec = this.firstIpDec + 1;
                  this.lastIp = new $.IPv4Address(this.lastIpDec, $.IP_DEC);
              }
          } else {
              // broadcast address
              this.bcIpDec = this.netIpDec + this.ipNum + 1;
              this.bcIp = new $.IPv4Address(this.bcIpDec, $.IP_DEC);
              // first ip
              this.firstIpDec = this.netIpDec + 1;
              this.firstIp = new $.IPv4Address(this.firstIpDec, $.IP_DEC);
              // last ip
              this.lastIpDec = this.netIpDec + this.ipNum;
              this.lastIp = new $.IPv4Address(this.lastIpDec, $.IP_DEC);
          }
      };
      this.isValidIp = function(ip) {
          /*
           * Return true if the given address in amongst the usable addresses,
           * or if the given CIDR is contained in this one.
           */
          if (!(ip instanceof $.IPv4Address) && !(ip instanceof $.CIDR)) { 
              if (ip.indexOf('/') == -1) {
                  ip = new $.IPv4Address(ip);
              } else {
                  ip = new $.CIDR(ip);
              }
          }
          if (ip instanceof $.IPv4Address) {
              return ip.ipDec >= this.firstIpDec && ip.ipDec <= this.lastIpDec;
          } else if (ip instanceof $.CIDR) {
              return ip.firstIpDec >= this.firstIpDec && ip.lastIpDec <= this.lastIpDec;        
          } else {
              return false;
          }
      };
      this.getIp = function() {
          /* Return the given address. */
          return this.ip;
      };  
      this.getNetmask = function() {
          /* Return the netmask. */
          return this.nm;
      };
      this.getFirstIp = function() {
          /* Return the first usable IP address. */
          return this.firstIp;
      };
      this.getLastIp = function() {
          /* Return the last usable IP address. */
          return this.lastIp;
      };
      this.getNetworkIp = function() {
          /* Return the network address. */
          return this.netIp;
      };
      this.getBroadcastIp = function() {
          /* Return the broadcast address. */
          return this.bcIp;
      };
      this.getIpNumber = function() {
          /* Return the number of usable IP addresses. */
          return this.ipNum;
      };
      this.toString = function() {
          return this.netIp + "/" + this.nm;
      };
      this.equals = function(other) {
          /* Compare this CIDR object with another object */
          return String(this) == String(other);
      }
      this.set(cidr);
  }

})(this);