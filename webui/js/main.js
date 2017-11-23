

var update = function(){

	var src;
	src = 'data.json';
	src = 'http://10.10.0.20:8888';
	$.getJSON(src , function(data){
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

setInterval(update, 1000);
