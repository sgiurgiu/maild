
function retrieve_and_show_mail() {
    var username = $('#username').val();
	$("#collapse_email_list").empty();
	$("#collapse_email_list").append("<p>Retrieving emails...</p>");
	$.get('/api/mails/'+username, function( data ) {
		$("#collapse_email_list").empty();
		if(data.length <= 0)
		{
			$("#collapse_email_list").append("<p>No emails available</p>");
		}
        for(var i=0;i<data.length;++i)
        {
            var from = data[i]['from'];
            var id = data[i]['id'];            
            var subject = data[i]['subject'];;
            
            var date_received = data[i]['date'];
            var mom_date = moment.utc(date_received, "YYYY-MM-DD HH:mm:ss.SSS");
			var local_date = mom_date.local().format('dddd, MMMM Do YYYY, HH:mm:ss');
			//<div class="card">  <div class="card-header"> 
			var htmlToInsert = '<div class="card">  <div class="card-header">';
			
			htmlToInsert+='<table class="table table-sm mb-0 table-borderless"><tbody>';
			htmlToInsert+='<tr class="selectable" id="heading'+id+'" data-toggle="collapse" data-target="#email_body_'+id+'">';
			htmlToInsert+='<td  width="20%">'+from+'</td><td width="65%">'+subject+'</td> <td width="15%">'+local_date+'</td></tr>';

			htmlToInsert+='</tbody></table></div>';
			

			htmlToInsert+='<div id="email_body_'+id+'" class="collapse" role="tabpanel" aria-labelledby="heading'+id+'" data-parent="#collapse_email_list">';

			var html_tabs = '<div class="card-body">';
			html_tabs += '<ul class="nav nav-tabs" role="tablist">';        
			html_tabs += '<li class="nav-item" role="presentation"><a id="html_tab_'+id+'" class="nav-link active" href="#html_part'+id+'" aria-selected="true" aria-controls="html_part'+id+'" role="tab" data-toggle="tab">Html</a></li>';
			html_tabs += '<li class="nav-item" role="presentation"><a id="text_tab_'+id+'" class="nav-link" href="#text_part'+id+'" aria-selected="false" aria-controls="text_part'+id+'" role="tab" data-toggle="tab">Text</a></li>';
			html_tabs += '<li class="nav-item" role="presentation"><a id="raw_tab_'+id+'" class="nav-link" href="#raw_part'+id+'" aria-selected="false" aria-controls="raw_part'+id+'" role="tab" data-toggle="tab">Raw</a></li>';        
			html_tabs += '</ul>';
			html_tabs += '<div class="tab-content" >';			
            html_tabs += '    <div role="tabpanel" style="height: 500px;" class="tab-pane show active" aria-labelledby="html_tab_'+id+'" id="html_part'+id+'"><iframe  frameborder="0" border="0" cellspacing="0"  width="100%" height="100%"  src="/api/mail/'+id+'/html"/></div>';
            html_tabs += '    <div role="tabpanel" style="height: 500px;" class="tab-pane" aria-labelledby="text_tab_'+id+'" id="text_part'+id+'"><iframe frameborder="0" border="0" cellspacing="0"  width="100%" height="100%" style="border-style:none" src="/api/mail/'+id+'/text"/></div>';
            html_tabs += '    <div role="tabpanel" style="height: 500px;" class="tab-pane" aria-labelledby="raw_tab_'+id+'" id="raw_part'+id+'"><iframe  frameborder="0" border="0" cellspacing="0"  width="100%" height="100%" style="border-style:none" src="/api/mail/'+id+'/raw"/></div>';
			html_tabs += '</div>';
			html_tabs += '</div>';        

			htmlToInsert+=html_tabs;

			htmlToInsert+='</div>';
			$('#collapse_email_list').			
			append(htmlToInsert);
        }
        
        $('#collapse_email_list').collapse('show');    
    });        
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
