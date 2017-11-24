

class vnf_perf_chart {

	constructor (name, opts) {
		this.name = name;
		this.opts = opts;
		this.chart = c3.generate(opts);
	}

	update() {
		var rxrate_col = (this.opts.data.columns[0]);
		var perf_col   = (this.opts.data.columns[1]);

		var _this2 = this;
		_this2.chart.load({
			columns : [
				rxrate_col,
				perf_col,
			]
		});
	}

};

var create_vnf_graph = function(vnfname) {
	console.log("bindto: #" + vnfname);

	var opts = {
		bindto: "#" + vnfname,
		size: {
			height : 200,
			width  : 400,
		},
		transition: {
			duration : 0,
		},
		data: {
			columns: [
				[
					'rxrate',
					0, 0, 0, 0, 0,
					0, 0, 0, 0, 0,
					0, 0, 0, 0, 0,
				],
				[
					'perf',
					0, 0, 0, 0, 0,
					0, 0, 0, 0, 0,
					0, 0, 0, 0, 0,
				],
			],
			types: {
				rxrate: 'bar',
				perf: 'spline',
			},
		},
		axis: {
			y: {
				max: 20000,
				min: 1000,
				tick: {
					culling: {
						max: 4
					}
				}
			}
		},
	}

	var g = new vnf_perf_chart(name, opts);
	setInterval(function(){
		g.update();
	},1000);

	setInterval(function(){

		var append_to_col = function(col, data) {
			col.push(data);
			if (col.length > 15) {
				var name = col.shift();
				col[0] = name;
			}
		}

		var URL = 'http://10.0.0.3:8888/vnfs/' + vnfname;
		$.getJSON(URL, function(data){

			var rxrate = Math.round(data.vnf.rxrate/1000);
			var perf = Math.round(rxrate * data.vnf.perfred);
			// console.log('--------------------------');
			// console.log('rxrate: ' + rxrate);
			// console.log('perf  : ' + perf  );

			var rxrate_col = (opts.data.columns[0]);
			var perf_col   = (opts.data.columns[1]);
			append_to_col(rxrate_col , rxrate);
			append_to_col(perf_col   , perf  );

		}); /* getJSON */

	},1000);
}

