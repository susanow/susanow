

var create_vnf_gauge = function(vnfname) {

	var opts = {
		bindto: "#" + vnfname + '-gauge',
		data: {
			columns: [ ['data', 50] ],
			type: 'gauge',
		},
		transition: {
			duration : 0,
		},
		gauge: {},
		color: {
			pattern: ['#FF0000', '#F97600', '#F6C600', '#60B044'], // the three color levels for the percentage values.
			threshold: {
				values: [30, 60, 90, 100]
			}
		},
		size: {
			width: 200,
		}
  }

	var chart = c3.generate(opts);

	setInterval(function(){
		var URL = 'http://10.0.0.3:8888/vnfs/' + vnfname;
		$.getJSON(URL, function(data){

			var perfred = Math.round(data.vnf.perfred*100);
			console.log("perfred: " + perfred);

			chart.load({
				columns : [[ 'data', perfred ]]
			});

		}); /* getJSON */
	}, 1000);
}

