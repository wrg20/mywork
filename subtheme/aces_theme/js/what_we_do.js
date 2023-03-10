// Create a new XMLHttpRequest object
let xhr = new XMLHttpRequest();

var aces_unit_id = drupalSettings.directory.aces_unit_id;

// Create the XML request URL
var xml_list_request = 'https://ws.engr.illinois.edu/directory/list.asp?unit=' + aces_unit_id + '&cat=15,9,25&template=2673'

// GET-request for the URL 
xhr.open('GET', xml_list_request);

// Send the request over the network
xhr.send();

// This will be called after the response is received
xhr.onload = function() {
  if (xhr.status != 200) { // analyze HTTP status of the response
    console.log("error 200");
  } 
  else { // show the result
    var output = "";
    var result = xhr.response.replace(/document.write\(\'/gi, '');
    result = result.replace(/\'\);/g, '');
    result = result.replace(/\\/g, '');
    result = result.replaceAll('<ul>', '');
    result = result.replaceAll('<li>', '');
    result = result.replaceAll('</li>', '');
    result = result.replaceAll('</ul>', '');
    result = result.substring(0, result.indexOf('function ShowTab('));
    result = result.replace(/\'\);/gi, '');

    var search_start = 0;
    var output = '<div class="aces--wwdwim">';
    var person_string = '';
    var even = false;

    search_result = result.indexOf('<div class="aces--wwdwim--person">', search_start);

    while (search_result > 0) {

      mission_start = search_result;
      mission_found = result.indexOf('<div class="aces--wwdwim--person-mission-statement">', mission_start);
      mission_end = result.indexOf('</div>', mission_found);

      if (mission_end - mission_found >= 55) {
        // It wasn't an empty mission statement
        search_end = result.indexOf('<!-- END person', search_result);
        person_string = result.substring(search_result, search_end);
        if (even) {
          person_string = person_string.replace('"aces--wwdwim--person"', '"aces--wwdwim--person aces--wwdwim--person--gray"');
        }
        output += person_string;
        even = !even;
      }


      search_start = search_result + 1;
      search_result = result.indexOf('<div class="aces--wwdwim--person">', search_start);

    }

    output += '</div>';
    WWDList = document.getElementsByClassName("WWDList");

    WWDList[0].innerHTML = output;

  }
};


xhr.onprogress = function(event) {

};

xhr.onerror = function() {
  alert("Request failed");
};
