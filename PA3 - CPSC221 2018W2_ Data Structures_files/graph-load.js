function load_viz(callback) {
    if (typeof Vis == 'undefined') {
        var base = $("meta[name='base_url']").attr("content");
        $.getScript(base + "/js/viz.min.js", function() {
            callback();
        });
    } else {
        callback();
    }
}
function load_graphs() {
    function transformDot(dot) {
        var index = dot.indexOf("{");
        if (index >= 0) {
            // dot = dot.slice(0, index+1) + 'graph [bgcolor="pink"];' + "\n" + dot.slice(index+1);
            dot = dot.slice(0, index+1) + 'bgcolor="transparent";' + "\n" + dot.slice(index+1);
        }
        return dot;
    }

    var graphNum = 0;
    function drawFirstGraph() {
        var graphs = $("script[type='x/graph']");
        if (graphs.length) {
            var $ele = $(graphs[0]);
            var id = "graph-"+graphNum;
            graphNum++;

            var format = "svg";
            var engine = $ele.data("engine") || undefined;

            var dot = $ele.text();
            dot = transformDot(dot);

            try {
                result = Viz(dot, format, engine);
            } catch (e) {
                console.log(e);
                return;
            }

            if (window.localStorage) {
                var hash = "engine=" + engine + "  " + dot;
                window.localStorage[hash] = result;
            }

            var $result = $(result);
            $result.attr("id", id);
            $result.attr("class", "graph");

            $ele.after($result);
            $('#'+id+'-loading').remove();
            $ele.remove();
            if (graphs.length > 1) {
                setTimeout(drawFirstGraph, 1);
            }
        }
    }

    var needToLoad = true;
    if (window.localStorage) {
        needToLoad = false;
        $("script[type='x/graph']").each(function(i, ele) {
            var $ele = $(ele);
            var id = "graph-"+i;

            var dot = $ele.text();
            dot = transformDot(dot);

            var engine = $ele.data("engine") || undefined;
            var hash = "engine=" + engine + "  " + dot;

            if (window.localStorage[hash]) {
                var $result = $(window.localStorage[hash]);
                $result.attr("id", id);
                $result.attr("class", "graph");

                $ele.after($result);
                $('#'+id+'-loading').remove();
                $ele.remove();
            } else {
                needToLoad = true;
            }
        });
    }
    if (needToLoad) {
        if ($("script[type='x/graph']").length) {
            load_viz(drawFirstGraph);
        }
        $("script[type='x/graph']").each(function(i, ele) {
            var $ele = $(ele);
            var id = "graph-"+i+"-loading";
            $ele.after('<div id="'+id+'" class="graph-loading">Loading graph...</div>');
        });
    }
}
load_graphs();
