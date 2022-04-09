import Module from './quick_example.js';

onmessage = function(message) {
  const nbr = message.data;
  var n1 = 0;
  var n2 = 1;
  var somme = 0;
  var grphs = [];

  // const mod = Module().then((prop) => {

  //   var zk = prop.generate(dormans);
  // console.log(zk)
  //       });


  const mod = Module().then((prop) => {
    var dormans = message.data[0];
    var num_samples = message.data[1];
    var zk = prop.generate_n(dormans, num_samples);
    /* console.log("zk", zk.size()) */
    for (var i = 0; i < zk.size(); i++) {

      var sk = zk.get(i);
      var nodes = [];
      var edges = [];
      var map = {};

      for (var h = 0; h < sk.nodes.size(); h++) {
        var node = sk.nodes.get(h);
        /* console.log("whait")
* console.log(node.chain_str);
* console.log(node); */
        var node_cp = {
          inder: h, id: node.id, abbrev: node.abbrev, label: node.label
        }
        /* console.log(node.id); */
        map[node.id] = h;
        /* node.id = i; */
        nodes.push(node_cp);
        node.delete();

      }
      sk.nodes.delete();
      for (var es = 0; es < sk.edges.size(); es++) {
        /* var edge = {"from": sk.edges.get(i).from, "to" :sk.edges.get(i).to} */
        var temp_ed = sk.edges.get(es);
        var edge = { "from": map[temp_ed.from], "to": map[temp_ed.to] }
        /* console.log("edge") */
        /* console.log(edge) */
        edges.push(edge);
        temp_ed.delete();

      }
      sk.edges.delete();

      var k = { "nodes": nodes, "edges": edges }
      /* for (var j=0; j<k.edges.size();j++){
       *     console.log(k.edges.get(j));
       * } */
      /* console.log("chain_str", k["chain_str"]); * / */
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
    postMessage(grphs);
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
