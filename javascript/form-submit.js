$(document).ready(function(){
	$("#findLatLongBtn").click(function() {
		 getPlaceCoordinates();
	});

	$('#addform').submit(function () {
		var name = $.trim($('#formName').val());
		if (name === '') {
			alert('Name field is empty.');
			return false;
		}
		var dob = $.trim($('#formDateField').val());
		if ((dob === '') || (!isValidDate(dob))) {
			alert('Date field is empty or invalid.');
			return false;
		}
		var xlat = Number($.trim($('#formLatitude').val()));
		if ((xlat == NaN) || (xlat > 180) || (xlat < -180)) {
			alert('Latitude field is empty, invalid or out of range.');
			return false;
		}
		var xlng = Number($.trim($('#formLongitude').val()));
		if ((xlng == NaN) || (xlng > 180) || (xlng < -180)) {
			alert('Longitude field is empty, invalid or out of range.');
			return false;
		}
	});
});

function isValidDate(dateString) {
  	var regEx = /^\d{4}-\d{2}-\d{2}$/;
  	if (!dateString.match(regEx)) return false;  
  	var d = new Date(dateString);
	if (Number.isNaN(d.getTime())) return false; 
	// no further checks  
  	return true;
}

function getPlaceCoordinates() {
	var address = $("#formAddress").val();
	// we are using MapQuest's Nominatim service
	var geocode = 'https://open.mapquestapi.com/geocoding/v1/address?key=some-key&format=json&location=' + address;
	// use jQuery to call the API and get the JSON results
	$.getJSON(geocode, function(data) {
		// get lat + lon from first match
		var latitude = data.results[0].locations[0].latLng.lat;
   		var longitude = data.results[0].locations[0].latLng.lng;
    	console.log("address="+address+", longitude="+longitude);
    	$("#addxlat").val(latitude);
    	$("#addxlng").val(longitude);
	});
}
