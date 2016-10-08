
function retrieve_and_show_mail() {
    var username = $('#username').val();
    $("#email_list > tbody").empty();
    $.get( '/api/mails/'+username, function( data ) {        
        for(var i=0;i<data.length;++i)
        {
            var from = data[i]['from'];
            var id = data[i]['id'];            
            var subject = data[i]['subject'];;
            
            var date_received = data[i]['date'];
            var mom_date = moment.utc(date_received, "YYYY-MM-DD HH:mm:ss.SSS");
            var local_date = mom_date.local().format('dddd, MMMM Do YYYY, HH:mm:ss');
            $('#email_list > tbody:last-child').append('<tr class="selectable" onclick="showEmailContents('+id+')"> <td>'+from+'</td><td>'+subject+'</td> <td>'+local_date+'</td></tr>');
            $('#email_list > tbody:last-child').append('<tr style="display: none" id="email_body_'+id+'" > <td colspan="3" id="email_body_td_'+id+'">  <img src="/img/loading_spinner.gif"> </td></tr>');
        }
        
        $('#collapse_email_list').collapse('show');    
    });        
}

function showEmailContents(id) {    
    $('#email_body_'+id).toggle();    
    
        var html_tabs = '<div >';
        html_tabs += '<ul class="nav nav-tabs" role="tablist">';        
        html_tabs += '<li role="presentation"><a href="#html_part'+id+'" aria-controls="html_part'+id+'" role="tab" data-toggle="tab">Html</a></li>';
        html_tabs += '<li role="presentation"><a href="#text_part'+id+'" aria-controls="text_part'+id+'" role="tab" data-toggle="tab">Text</a></li>';
        html_tabs += '<li role="presentation"><a href="#raw_part'+id+'" aria-controls="raw_part'+id+'" role="tab" data-toggle="tab">Raw</a></li>';        
        html_tabs += '</ul>';
        html_tabs += '<div class="tab-content" >';
        
        html_tabs += '    <div role="tabpanel" style="height: 500px;" class="tab-pane" id="html_part'+id+'"><iframe width="100%" height="100%" src="/api/mails/'+id+'/html"/></div>';        
        html_tabs += '    <div role="tabpanel" style="height: 500px;" class="tab-pane" id="text_part'+id+'"><iframe width="100%" height="100%" src="/api/mails/'+id+'/text"/></div>';        
        html_tabs += '    <div role="tabpanel" style="height: 500px;" class="tab-pane" id="raw_part'+id+'"><iframe width="100%" height="100%" src="/api/mails/'+id+'/raw"/></div>';        
        html_tabs += '</div>';
        html_tabs += '</div>';        
        
        $('#email_body_td_'+id).html(html_tabs);        
        $('.nav-tabs a[href="#html_part'+id+'"').tab('show');                    
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
