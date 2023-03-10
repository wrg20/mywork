if (typeof program != 'undefined' && program == 'program_explorer') {
	// Displaying the list of courses

							jQuery('.ilfw--program-explorer').attr('style','display: none !important');
	jQuery(document).ready(function(){ 
							jQuery('.ilfw--program-explorer').attr('style','display: none !important');

/*
{% set actions = view.args[0]|split('|') %}
{% for action in actions %}
	{% set rows = action|split(':') %}
	{% for row in rows %}
		{% if loop.index == 1 %}
			<h3>Action: {{ row }}</h3>
		{% else %}
			{% set line = row|split(';') %}
			<ul>
				<li>
					<strong>Option type: {{ line[0] }}: </strong>Detail: {{ line[1] }}
				</li>
			</ul>
		{% endif %}
	{% endfor %}
{% endfor %}
*/

	 	var perfEntries = performance.getEntriesByType("navigation");

		if (perfEntries[0].type === "back_forward") {
		    jQuery("#clear-filter").trigger('change');
		};

		jQuery('[id^="edit-field-program-format-value-online"]').prop('checked', true).trigger('change');

		Drupal.behaviors.betterExposedFilters = {
	    attach: function(context, settings) {  
	    	jQuery("#clear-filter").click(function() {
	    		jQuery(".form-text").val("");
	    		jQuery(".form-checkbox").prop("checked", false);
	    		jQuery(".form-select").val('All');
			    jQuery(".form-select").trigger('change');
	    	});

				var num_results = jQuery('.ilfw--program-explorer--card--container').length;

				result_text = 'Results (' + num_results + ')';
				if (num_results == 0) {
					jQuery(".ilfw--program-explorer--card-list").html('<div class="ilfw--program-explorer--no-results">No results were found based on your selection. Please unselect some of the filters used to refine your results.</div>');
				}

				jQuery("h3[role='status']").text(result_text);

				jQuery(".ilfw--program-explorer--refine-results").click(function() {
					console.log('Refine Results clicked');
					// Change chevron direction
					if (jQuery(".ilfw--program-explorer--refine-results--header").hasClass("fa-chevron-down")) {
						jQuery(".ilfw--program-explorer--refine-results--header").removeClass("fa-chevron-down").addClass("fa-chevron-up");
					}
					else {
						jQuery(".ilfw--program-explorer--refine-results--header").removeClass("fa-chevron-up").addClass("fa-chevron-down");
					}

					jQuery(".bef-exposed-form").toggle();
		  	});

jQuery('.ilfw--program-explorer').attr('style','display: none !important');


console.log(filter_options);
				current_action = '';

				// Split out between Hide and Select sections
				var actions = filter_options.split("|");
				actions.forEach((action) => {
					// Split out each entry in the current section
					var rows = action.split(":");
					rows.forEach((row, index) => {
						// The first entry in the row is always the action to be performed (Hide or Select)
						if (index === 0) {
							// Keep track of the action we're performing
							console.log('Action = ' + row);
							current_action = row;
						}
						// Process each entry for the current action
						else {
							var options = row.split(";");
							console.log('Option type: ' + options[0] + ' Detail: ' + options[1]);
							// If it's a Hide action, set the given entries to display: none
							if (current_action == 'Hide') {
								// If it's Hide All, then we'll hide the whole filter option set
								if (options[1] == 'All') {
									jQuery('[id^="' + options[0] + '"]').hide();
									console.log('hide: ' + options[0]);
								}
								else {
									var details = options[1].split(",");
									var option = options[0].replace("edit-","");
									details.forEach((detail) => {
										jQuery(".form-item-" + option + "-" + detail).hide();
//										console.log('New: ' + ".form-item-" + option + "-" + detail);
//										console.log('Not all: ' + '[class^="form-item-' + option + '-' + detail + '"]');
									});
								}
							}
							else {
								console.log('Not Hide: ' + current_action)
							}
						}
					});
				});

if (jQuery(window).width() >= 1100) {
	jQuery('.ilfw--program-explorer').attr('style','display: flex');
}
else {
	jQuery('.ilfw--program-explorer').attr('style','display: block');
}
console.log('width = ' + jQuery(window).width());
				console.log(filter_options);

			}
	 	};

		jQuery("#views-exposed-form-programs-block-full-program-explorer").submit(function(e) {
			e.preventDefault();
		});

		var num_results = jQuery('.ilfw--program-explorer--card--container').length;

		result_text = 'Results (' + num_results + ')';
		if (num_results == 0) {
			jQuery(".ilfw--program-explorer--card-list").html('<div class="ilfw--program-explorer--no-results">No results were found based on your selection. Please unselect some of the filters used to refine your results.</div>');
		}

		jQuery("h3[role='status']").text(result_text);

		jQuery( window ).resize(function() {
			// Make sure filters display on the side when at desktop screen size
  		if (innerWidth >= 1100) {
  			var filter_div = getElementByID("views-exposed-form-programs-page-1");
  			filter_div.style.display = 'flex';
  		}
		});

		jQuery(".ilfw--program-explorer--refine-results").click(function() {
			console.log('Refine Results clicked');
			// Change chevron direction
			if (jQuery(".ilfw--program-explorer--refine-results--header").hasClass("fa-chevron-down")) {
				jQuery(".ilfw--program-explorer--refine-results--header").removeClass("fa-chevron-down").addClass("fa-chevron-up");
			}
			else {
				jQuery(".ilfw--program-explorer--refine-results--header").removeClass("fa-chevron-up").addClass("fa-chevron-down");
			}

			jQuery(".bef-exposed-form").toggle();
  	});
	});
}
