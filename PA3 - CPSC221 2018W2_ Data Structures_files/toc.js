function load_toc() {
    var $window = $(window);
    var throttleRate = 50;
    var $elements;
    var m = {}; // maps from #id to nav element
    var $toc_items = $();
    var $$elements = [];
    function toc_init() {
        $elements = $("h2,h3", "body");

        m = {};
        $toc_items = $("#toc li");
        $toc_items.each(function(i, e) {
            m[$(e).children("a").attr("href")] = $(e);
        });


        $$elements = [];
        for (var i = 0; i < $elements.length; i++) {
            $$elements.push($($elements[i]));
        }

        $("h2,h3,h4,h5,h6", "body").each(function(i, e) {
            var $e = $(e);
            var id = $e.attr("id");
            if (id) {
                var link = document.createElement("a");
                link.href = "#" + id;
                link.className = "header-anchor";
                link.innerHTML = "";
                $e.append(link);
            }
        });
    }

    /**
     * Get time in ms
     * @license https://raw.github.com/jashkenas/underscore/master/LICENSE
     * @type {function}
     * @return {number}
     */
    var getTime = (Date.now || function () {
        return new Date().getTime();
    });


    /**
     * Returns a function, that, when invoked, will only be triggered at most once
     * during a given window of time. Normally, the throttled function will run
     * as much as it can, without ever going more than once per `wait` duration;
     * but if you'd like to disable the execution on the leading edge, pass
     * `{leading: false}`. To disable execution on the trailing edge, ditto.
     * @license https://raw.github.com/jashkenas/underscore/master/LICENSE
     * @param {function} func
     * @param {number} wait
     * @param {Object=} options
     * @returns {Function}
     */
    function throttle(func, wait, options) {
        var context, args, result;
        var timeout = null;
        var previous = 0;
        options || (options = {});
        var later = function () {
            previous = options.leading === false ? 0 : getTime();
            timeout = null;
            result = func.apply(context, args);
            context = args = null;
        };
        return function () {
            var now = getTime();
            if (!previous && options.leading === false) previous = now;
            var remaining = wait - (now - previous);
            context = this;
            args = arguments;
            if (remaining <= 0) {
                clearTimeout(timeout);
                timeout = null;
                previous = now;
                result = func.apply(context, args);
                context = args = null;
            } else if (!timeout && options.trailing !== false) {
                timeout = setTimeout(later, remaining);
            }
            return result;
        };
    };

	function onScroll() {
        var top = $window.scrollTop();
        var height = $window.height();
        var $firstInView = $$elements[0];
        var firstInViewVisible = false;
        var $visible = [];
		$.each($$elements, function(i, $element) {
            var eTop = $element.offset().top;
            var eHeight = $element.height();
            if (eTop-eHeight*0.75 <= top+50
                    || (!firstInViewVisible && eTop-eHeight*0.75 <= top+height/3)
                    ) {
                if (eTop+eHeight*0.5 > top) {
                    firstInViewVisible = true;
                } else {
                    firstInViewVisible = false;
                }
                $firstInView = $element;
            }
            if ($element.offset().top+$element.height()*0.75 >= top-50) {
                if (eTop+$element.height() <= top+height+50) {
                    $visible.push($element);
                } else {
                    return false;
                }
            }
		});

        if ($firstInView) {
            var $toc_item = m['#'+$firstInView[0].id];
            if ($toc_item) {
                $toc_items.removeClass("active");
                $toc_item.addClass("active");
            }
        }

        $toc_items.removeClass("visible");
        for (var i = 0; i < $visible.length; i ++) {
            var $toc_item = m['#'+$visible[i][0].id];
            if ($toc_item) {
                $toc_item.addClass("visible");
            }
        }
	}

    var throttleScroll = throttle(onScroll, throttleRate);

    $window
        .on('scroll', throttleScroll)
        .on('resize', throttleScroll);
    setTimeout(throttleScroll, 0);

    toc_init();
}

load_toc();
