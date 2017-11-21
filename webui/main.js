
$(function(){

	$.getJSON('http://10.10.0.20:8888' , function(data){
		n_vnf  = data.n_vnf;
		n_port = data.n_port;
		n_vcat = data.n_vcat;
		n_pcat = data.n_pcat;
		document.write('n_vnf : ' + n_vnf  + '<br>');
		document.write('n_port: ' + n_port + '<br>');
		document.write('n_vcat: ' + n_vcat + '<br>');
		document.write('n_pcat: ' + n_pcat + '<br>');

		for (var i=0; i<n_vnf; i++) {
			document.write('vnf[' + i + '] <br>');
		}

		document.write('<br><br>');
	});

	$.getJSON('http://10.10.0.20:8888/vnfs/vnf0' , function(data){
		document.write(JSON.stringify(data));
	});

});

