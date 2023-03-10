if (typeof view != 'undefined' && view == 'explorer') {
	// Displaying the list of courses
	var aces_course_code = drupalSettings.courses.aces_course_code;
	const all_courses = {};
	let master_course_list = [];
  let term_array = [1, 5, 8];
	let term_print_array = ['Spring', 'Summer', 'Fall'];
	let term_list = [];
	let past_term_string = [];
	let future_term_string = [];

	let term_courses = [];
	let course_array = [];

	term_string = '1' + current_year + term_array[term_index];
	search_string = "https://ws.engr.illinois.edu/courses/list.asp?template=2680&output=html&subj=" + aces_course_code + '&term=' + term_string;
	term_list.push(term_string);

	jQuery.ajax({url: search_string, success: function(result) {
		result = result.replace(/document.write\(\'/g, '');
		result = result.replace(/\'\);/g, '');
		result = result.replace(/\\/g, '');
		result = result.replace('class="row"', '');
		result = result.replace(/<.*">/g, '');
		result = result.replace(/<\/div>/g, '');
		result = result.replace(/\\r\\n/g, '');

		all_courses[term_string] = [];
		let temp_array = [];

		term_courses = result.split('{');
		term_courses.forEach(function (course, index) {
			if (course != '\r\n') {
				course_array = course.split('|');
				temp_array.push(course_array);
			}
		});


		var sorted_array = temp_array.sort(function(a, b) {
		  return a[1] - b[1];
		});

		temp_array = sorted_array;
		sorted_array = [];

		sorted_array = temp_array.sort(function(a, b) {
		  return a[0].localeCompare(b[0]);
		});

		temp_array = sorted_array;
		all_courses[term_string] = temp_array;

	}});

	const future_term_courses = {};
	const future_course_array = {};
	const future_temp_array = {};
	const future_sorted_array = {};

  for (let term = 1; term <= latest_term; term++) {
    print_term = (term_index + term) % 3;
    print_year = parseInt(current_year) + Math.floor((parseInt(term_index) + parseInt(term)) / 3);
		future_term_string[term] = '1' + print_year + term_array[print_term];
	  search_string = "https://ws.engr.illinois.edu/courses/list.asp?template=2680&output=html&subj=" + aces_course_code + '&term=' + future_term_string[term];
		term_list.push(future_term_string[term]);

  	jQuery.ajax({url: search_string, success: function(result) {
		  result = result.replace(/document.write\(\'/g, '');
		  result = result.replace(/\'\);/g, '');
		  result = result.replace(/\\/g, '');
			result = result.replace('class="row"', '');
			result = result.replace(/<.*">/g, '');
			result = result.replace(/<\/div>/g, '');
			result = result.replace(/\\r\\n/g, '');

			all_courses[future_term_string[term]] = [];
			future_temp_array[future_term_string[term]] = [];

			future_term_courses[future_term_string[term]] = result.split('{');
			future_term_courses[future_term_string[term]].forEach(function (course, index) {
				if (course != '\r\n') {
					future_course_array[future_term_string[term]] = course.split('|');
					future_temp_array[future_term_string[term]].push(future_course_array[future_term_string[term]]);
				}
			});

			future_sorted_array[future_term_string[term]] = [];

			future_sorted_array[future_term_string[term]] = future_temp_array[future_term_string[term]].sort(function(a, b) {
			  return a[1] - b[1];
			});

			future_temp_array[future_term_string[term]] = future_sorted_array[future_term_string[term]];
			future_sorted_array[future_term_string[term]] = [];

			future_sorted_array[future_term_string[term]] = future_temp_array[future_term_string[term]].sort(function(a, b) {
			  return a[0].localeCompare(b[0]);
			});

			future_temp_array[future_term_string[term]] = future_sorted_array[future_term_string[term]];

			all_courses[future_term_string[term]] = future_temp_array[future_term_string[term]];

		}});
 	}

	const past_term_courses = {};
	const past_course_array = {};
	const past_temp_array = {};
	const past_sorted_array = {};

  for (let term = oldest_term; term >= 1; term--) {
	  print_term = (3 + term_index - term) % 3;
	  if (term > term_index) {
	    print_year = current_year - 1 - Math.floor((term - term_index - 1) / 3);
	  }
	  else {
	    print_year = current_year;
	  }
	  past_term_string[term] = '1' + print_year + term_array[print_term];
	  search_string = "https://ws.engr.illinois.edu/courses/list.asp?template=2680&output=html&subj=" + aces_course_code + '&term=' + past_term_string[term];
		term_list.push(past_term_string[term]);

  	jQuery.ajax({url: search_string, success: function(result) {
		  result = result.replace(/document.write\(\'/g, '');
		  result = result.replace(/\'\);/g, '');
		  result = result.replace(/\\/g, '');
			result = result.replace('class="row"', '');
			result = result.replace(/<.*">/g, '');
			result = result.replace(/<\/div>/g, '');
			result = result.replace(/\\r\\n/g, '');

			all_courses[past_term_string[term]] = [];
			past_temp_array[past_term_string[term]] = [];

			past_term_courses[past_term_string[term]] = result.split('{');
			past_term_courses[past_term_string[term]].forEach(function (course, index) {
				if (course != '\r\n') {
					past_course_array[past_term_string[term]] = course.split('|');
					past_temp_array[past_term_string[term]].push(past_course_array[past_term_string[term]]);
				}
			});

			past_sorted_array[past_term_string[term]] = [];

			past_sorted_array[past_term_string[term]] = past_temp_array[past_term_string[term]].sort(function(a, b) {
			  return a[1] - b[1];
			});

			past_temp_array[past_term_string[term]] = past_sorted_array[past_term_string[term]];
			past_sorted_array[past_term_string[term]] = [];

			past_sorted_array[past_term_string[term]] = past_temp_array[past_term_string[term]].sort(function(a, b) {
			  return a[0].localeCompare(b[0]);
			});

			past_temp_array[past_term_string[term]] = past_sorted_array[past_term_string[term]];

			all_courses[past_term_string[term]] = past_temp_array[past_term_string[term]];

		}});
 	}


	jQuery(document).ajaxStop(function () {
		var course_found = false;

		term_list.forEach( function(term, index) {
			all_courses[term].forEach( function(course, index) {
				course_found = false;
				master_course_list.forEach( function(compare_course, index) {
					if (course[0] == compare_course[0] && course[1] == compare_course[1]) course_found = true;
				});
				if (!course_found) {
					master_course_list.push(course);
				}
			});
		});

		var sorted_array = master_course_list.sort(function(a, b) {
		  return a[1] - b[1];
		});

		master_course_list = sorted_array;
		sorted_array = [];

		sorted_array = master_course_list.sort(function(a, b) {
		  return a[0].localeCompare(b[0]);
		});

		master_course_list = sorted_array;
		jQuery('.course-explorer--course-table').html(create_table(master_course_list, 'All'));
	});

	function create_table(course_list, semester) {
		var course_table = '<table class="table table-responsive-md"><thead><tr><th>Number</th><th>Title</th><th>Catalog Info</th><th>Online</th><th>In Person</th></tr></thead><tbody>';
		course_list.forEach(function (course, index) {
			// Course number
			course_table += '<tr>	<td class="rubric"><a href="/academics/courses/' + course[0] + course[1];
			if (semester != 'All') {
				course_table += '/' + course[2] + '/' + course[3];
			}
			course_table += '">' + course[0] + ' ' + course[1] + '</a></td>';
			// Course title
			course_table += '<td class="title"><a href="/academics/courses/' + course[0] + course[1];
			if (semester != 'All') {
				course_table += '/' + course[2] + '/' + course[3];
			}
			course_table += '">' + course[6] + '</a></td>';
			// Catalog link
			course_table += '<td class="schedule"><a target="_blank" href="https://courses.illinois.edu/schedule/' + course[2] + '/' + course[3] + '/' + course[0] + '/' + course[1] + '"><i class="fa fa-external-link" aria-hidden="true"></i></a></td>';
			// Online icon
			course_table += '<td class="online">';
			if (course[4] == '1') {
				course_table += '<i class="fa fa-desktop" aria-hidden="true"></i>'
			}
			course_table += '</td>';
			// In person icon
			course_table += '<td class="oncampus">';
			if (course[5] == '1') {
				course_table += '<i class="fa fa-users"></i>'
			}
			course_table += '</td>';
			course_table += '</tr>';
		});
		course_table += '</tbody></table>';
		return course_table;
	}

//	jQuery('.course-explorer--course-table').html(result);

	jQuery(document).ready(function(){		// User clicked on a semester (or all).  Show the results.
	   jQuery(".course-explorer--tab--link").click(function () {

	   		jQuery(".course-explorer--tab--li").removeClass("active");
	   		jQuery('.' + jQuery(this).attr("data-term")).addClass("active");

	   		// Clear result table first in case this takes a while to process
	   		if (jQuery(this).attr("data-term") == 'All') {
	   			jQuery('.course-explorer--course-table').html(create_table(master_course_list, jQuery(this).attr("data-term")));
	   		}
	   		else {
	  		  jQuery('.course-explorer--course-table').html(create_table(all_courses[jQuery(this).attr("data-term")], jQuery(this).attr("data-term")));
	   		}

		   return false;
	   });
	});

}
else if (typeof view != 'undefined') {
	// Displaying individual course pages
	if (year == '') {
		jQuery.ajax({url: "https://ws.engr.illinois.edu/courses/item.asp?template=2676&output=html&course=" + course_name, success: function(result) {
		  result = result.replace(/document.write\(\'/g, '');
		  result = result.replace(/\'\);/g, '');
		  result = result.replace(/\\/g, '');
		  result = result.replace(/\/profile/g, '');
		  result = result.replace('class="row"', '');
		  result = result.replace('<h2>Official Description</h2>', '');
		  var page_title = jQuery(document).prop('title');
			jQuery(document).prop('title', page_title + ' | ' + course_name);
		//  result = result.substring(0, result.indexOf('function ShowTab('));

			if (result.length > 100) {
			  jQuery('.course-output').html(result);
				jQuery('.extCoursesProfileContent li').each(function() {
					term_offered = jQuery(this).text();
					term_offered_array = term_offered.split(' ');
					new_term_offered = '<a href="/academics/courses/' + course_name + '/' + term_offered_array[1] + '/' + term_offered_array[0] + '">' + term_offered + '</a>';
					jQuery(this).html(new_term_offered);
				});
				jQuery('.aces--course--timetable--overview').prepend('<h2>Course Details</h2>');
			};
		}});
	}
	else {
		var semester_code = "8";

console.log(semester);
		switch(semester) {
			case 'Spring':
				semester_code = "1";
				break;
			case 'Summer':
				semester_code = "5";
				break;
			case 'Fall':
				semester_code = "8";
				break;
		}
		var course_search = course_name + "-1" + year + semester_code;

		jQuery.ajax({url: "https://ws.engr.illinois.edu/courses/item.asp?template=2677&output=html&course=" + course_search, success: function(result) {
		  result = result.replace(/document.write\(\'/g, '');
		  result = result.replace(/\'\);/g, '');
		  result = result.replace(/\\/g, '');
		  result = result.replace(/\/profile/g, '');
		  result = result.replace('class="row"', '');
		  result = result.replace('<h2>Official Description</h2>', '');

		  var semester_header = result.match(/<h3 class=\"aces--course--semester\">(.*)<\/h3>/);
		  var page_title = jQuery(document).prop('title');
			jQuery(document).prop('title', page_title + ' | ' + course_name);
		  result = result.replace(/<h2> Schedule and Instructors/, '<h2> Schedule and Instructors - ' + semester_header[1]);
		  result = result.replace(/<h3 class=\"aces--course--semester\">(.*)<\/h3>/, '');

			if (result.length > 100) {
			  jQuery('.course-output').html(result);
			  jQuery('.extCoursesTimeTable table').addClass('table-responsive-md');
			};
		}});
	}
}
