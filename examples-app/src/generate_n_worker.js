import Module from './quick_example.js';

onmessage = function(message) {
  // console.log('change')
  const nbr = message.data;
  // console.log("Sda");
  var n1 = 0;
  var n2 = 1;
  var somme = 0;

  const mod = Module().then((prop) => {
    var dormans = message.data[0];
    var num_samples = message.data[1];
    var begin = message.data[3];
    var end = message.data[4];
    var chain_strs = message.data[5];
    var grphs = [];
    var plearning = chain_strs.length !== 0
    // console.log(plearning)
    if (plearning) {
      var vec = new prop.StringList();
      for (var i = 0; i < chain_strs.length; i++) {
        vec.push_back(chain_strs[i]);
      }
      var zk = prop.generate_n_learnt_chains(dormans, num_samples, vec);
      vec.delete();
      for (var j = 0; j < zk.size(); j++) {

        var sk = zk.get(j);
        var nodes = [];
        var edges = [];
        var map = {};


        var sk_nodes = sk.nodes;
        for (var h = 0; h < sk_nodes.size(); h++) {
          var node = sk_nodes.get(h);
          var node_cp = {
            inder: h, id: node.id, abbrev: node.abbrev, label: node.label
          }
          map[node_cp.id] = h;
          nodes.push(node_cp);
          node.delete();
        }
        sk_nodes.delete();
        var sk_edges = sk.edges;
        if (sk_edges.size() === 0) {
          // console.log("whaaaaaaaaaaaaaaat")
        }
        for (var es = 0; es < sk_edges.size(); es++) {
          var temp_ed = sk_edges.get(es);
          var edge = { "from": map[temp_ed.from], "to": map[temp_ed.to] }
          edges.push(edge);
          temp_ed.delete();
        }
        sk_edges.delete();

        var k = { "nodes": nodes, "edges": edges }
        k.chain_str = sk["chain_str"]
        k.map_linearity = sk["map_linearity"];
        k.leniency = sk["leniency"];
        k.mission_linearity = sk["mission_linearity"];
        k.path_redundancy = sk["path_redundancy"];
        k.hash = sk["hash"];
        grphs.push(k);
        sk.delete();
      }
      zk.delete();

      postMessage({ graphs: grphs, id: message.data[2], begin: begin, end: end, learning: plearning, selection: message.data[6] });
    } else {
      var zk = prop.generate_n(dormans, num_samples);
      for (var i = 0; i < zk.size(); i++) {

        var sk = zk.get(i);
        var nodes = [];
        var edges = [];
        var map = {};

        for (var h = 0; h < sk.nodes.size(); h++) {
          var node = sk.nodes.get(h);
          var node_cp = {
            inder: h, id: node.id, abbrev: node.abbrev, label: node.label
          }
          map[node.id] = h;
          nodes.push(node_cp);
          node.delete();

        }
        sk.nodes.delete();
        for (var es = 0; es < sk.edges.size(); es++) {
          var temp_ed = sk.edges.get(es);
          var edge = { "from": map[temp_ed.from], "to": map[temp_ed.to] }
          edges.push(edge);
          temp_ed.delete();

        }
        sk.edges.delete();

        var k = { "nodes": nodes, "edges": edges }
        k.chain_str = sk["chain_str"]
        k.map_linearity = sk["map_linearity"];
        k.leniency = sk["leniency"];
        k.mission_linearity = sk["mission_linearity"];
        k.path_redundancy = sk["path_redundancy"];
        k.hash = sk["hash"];
        sk.delete();
        grphs.push(k);
      }
      zk.delete();
      console.log(grphs);
      postMessage({ graphs: grphs, id: message.data[2], begin: begin, end: end, learning: plearning, selection: message.data[6] });

    }

  });
};
