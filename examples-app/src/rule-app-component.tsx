import React, { createRef, useEffect, useContext } from 'react';
import FormControl from '@mui/material/FormControl';
import Box from '@mui/material/Box';
import InputLabel from '@mui/material/InputLabel';
import Button from '@mui/material/Button';
import MenuItem from '@mui/material/MenuItem';
import Select from '@mui/material/Select';
import GraphComponent from "./graph-component"
interface Props {
    callback: any
    ruleApp: any
    index: number
}
export default function RuleAppComponent(props: Props) {
    const [state, setState] = React.useState({selected_rule: props.ruleApp.selected_rule});

    function renderGraph(graph: any) {

        if (graph == null) {
            return <div></div>

        }
        return <GraphComponent graph={graph} width={400} manipulate={false} />


    }

    function makeAppRules(ruleNames: any) {

        var rules:any= [];
        for (var i = 0; i < ruleNames.length; i++) {
                        rules.push(<MenuItem value={i}> {ruleNames[i]}</MenuItem>
                        )

    }
        return rules;

    }
    function handleRuleChange(e: any) {

        console.log(e);
        setState({selected_rule: e.target.value})

    }
    function handleApplyRule(e: any) {
        console.log(e);
        props.callback(props.index,state.selected_rule);
        /* var rule_apps = state.rule_apps; */
        /* rule_apps.push("whoops") */
        /* setState({ graphs: graphs, x: state.x, y: state.y, mode: state.mode, original_graph: null, trained_graph: null, rule_apps: rule_apps, recipe: state.recipe }); */

    }
    return (
<div>
            <div className="graphs">
                {renderGraph(props.ruleApp.graph)}
            </div>
                <Box className="menu" sx={{ minWidth: 120 }}>
                <FormControl fullWidth>
                    <InputLabel id="demo-simple-select-label">Rules</InputLabel>
                    <Select
                        labelId="demo-simple-select-label"
                        id="demo-simple-select"
                        value={state.selected_rule}
                        label="Rule"
                        onChange={handleRuleChange}
                    >
                        {makeAppRules(props.ruleApp.rule_names)}
                    </Select>
                </FormControl>
            </Box>
            <Button onClick={handleApplyRule} variant="contained">Apply</Button>
        </div>

    )
}
