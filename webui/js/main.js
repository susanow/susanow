
// const URL = 'http://10.10.0.20:8888';
const URL = 'http://10.0.0.3:8888';

var already_registered = function(name, list){
	n_ele = list.length;
	for (var i=0; i<n_ele; i++) {
		text = list[i].children[0].children[0].innerText;
		// console.log('---------------');
		// console.log('text: [' + text + ']');
		// console.log('name: [' + name + ']');
		if (text == name) {
			return list[i]; // found
		}
	}
	// console.log("not found");
	return false; // not found
}

var update_vnf = function(){
	$.getJSON(URL + '/vnfs' , function(data){

		var n_vnf_ = data.n_vnf;
		for (var i=0; i<n_vnf_; i++) {
			var vnf = data[i];
			var name     = vnf.name;
			var running  = vnf.running?"True":"False";
			var coremask = '0x' + (vnf.coremask).toString(16);
			var rxrate   = (Math.round(vnf.rxrate  / 1000)).toString();
			var perfred  = (Math.round(vnf.perfred * 100)).toString();

			list = $('#vnf-list').find('li');
			list_ele = already_registered(name, list);
			if (list_ele == false) {
				var list = document.getElementById("vnf-list");
				var str =
					'<li>' +
					'	<div id="vnf-name" class="collapsible-header">' +
					'		<span id="name">NAME</span>' +
					'	</div>' +
					'	<div class="collapsible-body">' +
					'   name: <span id="name">NAME</span> <br>' +
					'   running: <span id="running">RUNNING</span> <br>' +
					'   coremask: <span id="coremask">COREMASK</span> <br>' +
					'   <br>' +
					'   <div class="row">' +
					'     <div class="col s12 m6">' +
					'       <div class="card blue-grey darken-1">' +
					'         <div class="card-content white-text">' +
					'           <span class="card-title"><span id="vnfperf">VNFPERF</span> Kpps</span>' +
					'           performance: <span id="perfred">PERFRED</span> %<br>' +
					'           rx rate: <span id="rxrate">RXRATE</span> Kpps<br>' +
					'         </div>' +
					'       </div>' +
					'     </div>' +
					'   </div>' +
					'   <div id=NAME-gauge>THIS-PLACE-SHOULD-BE-VIEWED-GRAPH</div>' +
					'   <br>' +
					'   <br>' +
					'   <div id=NAME-graph>THIS-PLACE-SHOULD-BE-VIEWED-GRAPH</div>' +
					'	</div>' +
					'</li>';
				str = str.replace(/NAME/g, name);
				str = str.replace(/RUNNING/g, running);
				str = str.replace(/COREMASK/g, coremask);
				str = str.replace(/PERFRED/g, perfred);
				str = str.replace(/RXRATE/g, rxrate);
				$(str).appendTo(list);

				create_vnf_graph(name);
				create_vnf_gauge(name);

			} else {
				node = $(list_ele);
				node.find('#running').html(running);
				node.find('#coremask').html(coremask);
				node.find('#perfred').html(perfred);
				node.find('#rxrate').html(rxrate);
				vnfperf = Math.round(vnf.rxrate * vnf.perfred/1000);
				vnfperf = vnfperf.toString();
				node.find('#vnfperf').html(vnfperf);
				// console.log(e);
				// console.log(list_ele);
				// n  = list_ele.childNodes;
				// console.log(n);
				// console.log(document);
			}
		}

	});
}

var update_port = function(){
	$.getJSON(URL + '/ports' , function(data){
		var n_ports = data.n_port;
		for (var i=0; i<n_port; i++) {
			var port = data[i];
			var name = port.name;

			list = $('#port-list').find('li');
			if (!already_registered(name, list)) {
				var list = document.getElementById("port-list");
				var str =
					'<li>' +
					'	<div class="collapsible-header">' +
					'		<span id="name">NAME</span>' +
					'	</div>' +
					'	<div class="collapsible-body">' +
					'		<p>Lorem ipsum dolor sit amet.</p>' +
					'	</div>' +
					'</li>';
				str = str.replace('NAME', name);
				$(str).appendTo(list);
			}
		}
	});
}

var update_system = function(){
	$.getJSON(URL , function(data){
		n_vnf  = data.n_vnf;
		n_port = data.n_port;
		n_vcat = data.n_vcat;
		n_pcat = data.n_pcat;

		document.getElementById("n_vnf" ).innerText = n_vnf;
		document.getElementById("n_port").innerText = n_port;
		document.getElementById("n_vcat").innerText = n_vcat;
		document.getElementById("n_pcat").innerText = n_pcat;
	});
}

var update = function(){
	update_system();
	update_vnf();
	update_port();
}

setInterval(update, 1000);

