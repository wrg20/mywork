var $ = jQuery;

if (typeof net_id !== 'undefined') {
	var aces_unit_id = drupalSettings.directory.aces_unit_id;
	var aces_flickr_api = drupalSettings.galleria.aces_flickr_api;
	$.ajax({url: "https://ws.engr.illinois.edu/directory/item.asp?unit=" + aces_unit_id + "&id=" + net_id + "&imgwidth=250&template=2643", success: function(result) {
	  result = result.replace(/document.write\(\'/g, '');
	  result = result.replace(/\'\);/g, '');
	  result = result.replace(/\\/g, '');
	  result = result.replace('class="row"', '');
	//  result = result.substring(0, result.indexOf('function ShowTab('));

		if (result.length > 100) {
		  $('.directory-output').html(result);
		};
	}});
}
else {
	console.log('not defined');
};

	