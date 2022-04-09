import Module from './quick_example.js';

onmessage = function(message) {
    const nbr = message.data;
    var grphs = [];

    // const mod = Module().then((prop) => {

    //   var zk = prop.generate(dormans);
    // console.log(zk)
    //       });


    const mod = Module().then((prop) => {
        var dormans = message.data[0];
        var num_samples = message.data[1];
        var i = message.data[2];
        var begin = message.data[3];
        var end = message.data[4];
        var cont = message.data[5];
        var recipe = message.data[6];

        var vec = new prop.recipe();
        for (var i = 0; i < recipe.length; i++) {
            var re = new prop.RecipeEntry();
            re.rule_name = recipe[i].rule.name;
            re.min = recipe[i].min;
            re.max = recipe[i].max;
            re.index = i;
            vec.push_back(re);
            re.delete();
        }
        var zk = prop.apply_recipe_n(dormans, vec, num_samples, cont);
        vec.delete();
        /* console.log("zk", zk.size()) */


        for (var i = 0; i < zk.size(); i++) {
            var sk = zk.get(i);
            var nodes = [];
            var edges = [];
            var map = {};
            var sk_nodes = sk.nodes;

            for (var h = 0; h < sk_nodes.size(); h++) {
                var node = sk_nodes.get(h);
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
            sk_nodes.delete();
            var sk_edges = sk.edges;
            for (var es = 0; es < sk_edges.size(); es++) {
                /* var edge = {"from": sk.edges.get(i).from, "to" :sk.edges.get(i).to} */
                var temp_ed = sk_edges.get(es);
                var edge = { "from": map[temp_ed.from], "to": map[temp_ed.to] }
                /* console.log("edge") */
                /* console.log(edge) */
                edges.push(edge);
                temp_ed.delete();

            }
            sk_edges.delete();

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
        postMessage({ graphs: grphs, begin: begin, end: end });
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
