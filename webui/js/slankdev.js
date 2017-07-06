


var count = 0;
setInterval(function() {
	count ++;
	$(function() {
		$.getJSON("ssn.json" , function(data) {
			document.getElementById('nb_threads').value = data.nb_threads;
			document.getElementById('nb_cpus'   ).value = data.nb_cpus;
			document.getElementById('nb_ports'  ).value = data.nb_ports;

			var d = document.getElementById("dom");
			while (d.firstChild) d.removeChild(d.firstChild);
			for (var i=0; i<data.vnfs.length; i++) {
				var div1 = document.createElement('div');
				var div2 = document.createElement('div');
				div1.textContent = 'hoge' + count + ' ' + data.vnfs[i].name;
				div2.textContent = 'states=' + data.vnfs[i].nb_stages;
				d.appendChild(div1);
				d.appendChild(div2);
			}
		})
		.error(function(jqXHR, textStatus, errorThrown) {
				console.log("Error: " + textStatus);
		});
	});
}, 1000);


