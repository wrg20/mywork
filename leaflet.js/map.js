jQuery(function ($) {
  // Add basemap tiles and attribution.
  var baseLayer = L.tileLayer('https://basemaps.cartocdn.com/light_all/{z}/{x}/{y}.png', {
    attribution: '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors, &copy; <a href="http://cartodb.com/attributions">CartoDB</a>'
  });

  // Create map and set center and zoom.
  var map = L.map('map', {
    scrollWheelZoom: true,
    center: [40.116421, -88.243385],
    zoom: 5,
    maxZoom: 8,
    minZoom: 2
  }).fitWorld();
  // Add basemap to map.
  map.addLayer(baseLayer);

  var addressPoints = [];

  /*
   * Pull the drupal view "points" and add each point to the addressPoints Array
   * Consume using JSON format
   */

  $.getJSON("/points", function(data) {

    for (var i = 0; i < data.features.length; i++) {
      var Lat = data.features[i].geometry.coordinates[0];
      var Lng = data.features[i].geometry.coordinates[1];
      var Title = data.features[i].properties.name;
      addressPoints.push([Lng, Lat, Title]);
    }

    // add the markerClusterGroup
    var markers = L.markerClusterGroup();

    // loop over the address points and add them to the map
    for (var i = 0; i < addressPoints.length; i++) {
      var a = addressPoints[i];
      var title = a[2];
      var marker = L.marker(new L.LatLng(a[0], a[1]), { title: title });
      marker.bindPopup(title);
      markers.addLayer(marker);
    }
    map.addLayer(markers);
    // Set path to marker image.

    // Set website domain for image paths
    var pathLocation = window.location.host;
    // alert(pathLocation);
    L.Icon.Default.imagePath = '/sites/'+pathLocation+'/themes/sitetheme/images/leaflet';

    // Add the icons to the map and set the default for size
    var pmpIcon = L.Icon.extend({
      options: {
        iconUrl: '/sites/'+pathLocation+'/themes/sitetheme/images/leaflet/marker-icon.png',
        iconRetinaUrl: '/sites/'+pathLocation+'/themes/sitetheme/images/leaflet/marker-icon-2x.png',
        iconSize: [25, 41],
        iconAnchor: [13, 40],
        popupAnchor: [1, -46]
      }
    });
  });
});
