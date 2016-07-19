
function retrieve_and_show_mail() {
    var username = $('#username').val();
    $("#email_list > tbody").empty();
    $.get( '/api/mails/'+username, function( data ) {
        
        for(var i=0;i<data.length;++i)
        {
            var body = data[i]['body'];
            var from = data[i]['from'];
            var id = data[i]['id'];
            var lines = body.split("\r\n");
            var subject = '';
            for(var j=0;j<lines.length;++j)
            {
                var line = lines[j];
                if(line.startsWith('Subject:'))
                {
                    subject = line.substring('Subject:'.length);
                    break;
                }
            }
            
            var date_received = data[i]['date'];
            var mom_date = moment.utc(date_received, "YYYY-MM-DD HH:mm:ss.SSS");
            $('#email_list > tbody:last-child').append('<tr class="selectable" onclick="showEmailContents('+id+')"> <td>'+from+'</td><td>'+subject+'</td> <td>'+mom_date.local().format('dddd, MMMM Do YYYY, HH:mm:ss')+'</td></tr>');
            $('#email_list > tbody:last-child').append('<tr style="display: none" id="email_body_'+id+'" > <td colspan="3"><pre>'+$('<div/>').text(body).html()+'</pre></td></tr>');
        }
        
        $('#collapse_email_list').collapse('show');    
    });        
}

function showEmailContents(id) {
    $('#email_body_'+id).toggle();    
}



$('.collapse').collapse();

if (!String.prototype.startsWith) {
	(function() {
		'use strict'; // needed to support `apply`/`call` with `undefined`/`null`
		var defineProperty = (function() {
			// IE 8 only supports `Object.defineProperty` on DOM elements
			try {
				var object = {};
				var $defineProperty = Object.defineProperty;
				var result = $defineProperty(object, object, object) && $defineProperty;
			} catch(error) {}
			return result;
		}());
		var toString = {}.toString;
		var startsWith = function(search) {
			if (this == null) {
				throw TypeError();
			}
			var string = String(this);
			if (search && toString.call(search) == '[object RegExp]') {
				throw TypeError();
			}
			var stringLength = string.length;
			var searchString = String(search);
			var searchLength = searchString.length;
			var position = arguments.length > 1 ? arguments[1] : undefined;
			// `ToInteger`
			var pos = position ? Number(position) : 0;
			if (pos != pos) { // better `isNaN`
				pos = 0;
			}
			var start = Math.min(Math.max(pos, 0), stringLength);
			// Avoid the `indexOf` call if no match is possible
			if (searchLength + start > stringLength) {
				return false;
			}
			var index = -1;
			while (++index < searchLength) {
				if (string.charCodeAt(start + index) != searchString.charCodeAt(index)) {
					return false;
				}
			}
			return true;
		};
		if (defineProperty) {
			defineProperty(String.prototype, 'startsWith', {
				'value': startsWith,
				'configurable': true,
				'writable': true
			});
		} else {
			String.prototype.startsWith = startsWith;
		}
	}());
}
