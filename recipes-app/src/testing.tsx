import React from 'react';
import './App.css';
import { Vega } from 'react-vega';



function App() {
  const spec: any = {
    "$schema": "https://vega.github.io/schema/vega-lite/v4.json",
    "description": "Google's stock price over time.",
    "data": {
      "values": [
        { "x": 177, "y": 5 },
        { "x": 100, "y": 4 },
        { "x": 200, "y": 1 },
        { "x": 300, "y": 7 },
        { "x": 400, "y": 10 },
        { "x": 450, "y": 7 },
        { "x": 500, "y": 12 },
        { "x": 550, "y": 11 },
        { "x": 600, "y": 4 },
        { "x": 700, "y": 6 },
        { "x": 800, "y": 0 },
        { "x": 930, "y": 2 }
      ]
    },
    "mark": "line",
    "encoding": {
      "x": { "field": "x", "type": "quantitative" },
      "y": { "field": "y", "type": "quantitative" }
    },
    "selection": {
      "select": {
        "type": "interval",
        "encodings": ["x"]
      }
    }
  }


  function handleHover(name: any, value: any) {
    console.log(name, value);
  }
  const signalListners = { "select": handleHover };
  return (
    <div className="App">
      <Vega spec={spec} signalListeners={signalListners} />
    </div>
  );
}

export default App;
