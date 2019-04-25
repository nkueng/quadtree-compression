// from http://purecss.io/layouts/tucked-menu-vertical/#
(function (window, document) {
    var WINDOW_CHANGE_EVENT = ('onorientationchange' in window) ? 'orientationchange':'resize';

    function toggleHorizontal() {
        [].forEach.call(
                document.getElementById('header').querySelectorAll('.menu-can-transform'),
                function(el){
                    el.classList.toggle('pure-menu-horizontal');
                    el.classList.toggle('pure-menu-scrollable');
                }
                );
    };

    function toggleMenu() {
        // set timeout so that the panel has a chance to roll up
        // before the menu switches states
        if (document.getElementById('header').classList.contains('open')) {
            setTimeout(toggleHorizontal, 500);
        }
        else {
            toggleHorizontal();
        }
        document.getElementById('header').classList.toggle('open');
        document.getElementById('menu-toggle').classList.toggle('x');
    };

    function closeMenu() {
        if (document.getElementById('header').classList.contains('open')) {
            toggleMenu();
        }
    }

    document.getElementById('menu-toggle').addEventListener('click', function (e) {
        toggleMenu();
    });

    window.addEventListener(WINDOW_CHANGE_EVENT, closeMenu);
})(this, this.document);
