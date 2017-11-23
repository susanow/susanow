
const URL = 'http://10.10.0.20:8888';

var already_registered = function(name, list){
	n_ele = list.length;
	for (var i=0; i<n_ele; i++) {
		text = list[i].children[0].innerText;
		if (text == name) {
			return true;
		}
	}
	return false;
}

var update_vnf = function(){
	$.getJSON(URL + '/vnfs' , function(data){

		var n_vnf_ = data.n_vnf;
		for (var i=0; i<n_vnf_; i++) {
			var vnf = data[i];
			var name = vnf.name;

			list = $('#vnf-list').find('li');
			if (!already_registered(name, list)) {
				var list = document.getElementById("vnf-list");
				var str =
					'<li>' +
					'	<div id="vnf-name" class="collapsible-header">' +
					'		NAME' +
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
					'		NAME' +
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

