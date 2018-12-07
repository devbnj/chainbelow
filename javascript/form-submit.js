// Useful UI JS functionality
// Uses jQuery
$(document).ready(function(){
    // use for searching an address
    // get the latitude and longitude
    // using Mapquest geo API
	$("#findLatLongBtn").click(function() {
		 getPlaceCoordinates();
	});

    // use for submitting a html form
    // the form mut have post and method in place
	$('#addform').submit(function () {
        var name = $.trim($('#formName').val());
        // check for empty names
		if (name === '') {
			alert('Name field is empty.');
			return false;
        }
        // check for dates when not a date input type
		var dob = $.trim($('#formDateField').val());
		if ((dob === '') || (!isValidDate(dob))) {
			alert('Date field is empty or invalid.');
			return false;
        }
        // check for latitude and longitude
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

// isValidDate check for date
function isValidDate(dateString) {
  	var regEx = /^\d{4}-\d{2}-\d{2}$/;
  	if (!dateString.match(regEx)) return false;  
  	var d = new Date(dateString);
	if (Number.isNaN(d.getTime())) return false; 
	// no further checks  
  	return true;
}

// getPlaceCoordinates 
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
