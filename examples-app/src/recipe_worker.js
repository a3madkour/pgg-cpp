import  Module  from './quick_example.js';

onmessage = function(message) {
   console.log('change')
    const nbr = message.data;
  console.log("Sda");
    var n1 = 0;
    var n2 = 1;
    var somme = 0;

  const mod = Module().then((prop) => {

    var dormans = message.data[0];
    var zk = prop.generate(dormans);
  console.log(zk)
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



    postMessage(result);
  };
