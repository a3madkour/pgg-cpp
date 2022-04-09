import Module from './quick_example.js';

onmessage = function(message) {
  const chain_strs = message.data[0]
  var selected_graphs = [];
  // const mod = Module().then((prop) => {

  //   var zk = prop.generate(dormans);
  // console.log(zk)
  //       });

  console.log(chain_strs)

  const mod = Module().then((prop) => {
    var dormans = message.data[1];
    var num_samples = message.data[2];
    var vec = new prop.StringList();
    for (var i = 0; i < chain_strs.length; i++) {
      vec.push_back(chain_strs[i]);
    }
    console.log("selected_graps")
    var zk = prop.generate_n_learnt_chains(dormans, num_samples, vec);
    vec.delete();
    var grphs = [];
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

    postMessage({ graphs: grphs, selected_graphs: selected_graphs });
  });

  // console.log("ssasd");
  // Module["onRuntimeInitalized"] = function(){
  //   console.log("hohoh")
  //   var zk = Module.generate(dormans);
  //   console.log(zk);
  // }




  // for (let i = 2; i <= nbr; i++) {
  //   somme = n1 + n2;


  //   n1 = n2;


  //   n2 = somme;
  // }


  const result = "hlaf";

  // console.log(result);



};
