function setSizes() {
  var myWidth = 0, myHeight = 0;
  if( typeof( window.innerWidth ) == 'number' ) {
    //Non-IE
    myWidth = window.innerWidth;
    myHeight = window.innerHeight;
  } else if( document.documentElement && ( document.documentElement.clientWidth || document.documentElement.clientHeight ) ) {
    //IE 6+ in 'standards compliant mode'
    myWidth = document.documentElement.clientWidth;
    myHeight = document.documentElement.clientHeight;
  } else if( document.body && ( document.body.clientWidth || document.body.clientHeight ) ) {
    //IE 4 compatible
    myWidth = document.body.clientWidth;
    myHeight = document.body.clientHeight;
  }
  //window.alert( 'Width = ' + myWidth );
  // window.alert( 'Height = ' + myHeight );
  var linkpoolHeight = document.getElementById('linkpool').clientHeight;
  var sideBarTdWidth = document.getElementById('sideBarTd').clientWidth;
  document.getElementById('content').setAttribute('style' , 'height:'+(myHeight-linkpoolHeight-100)+'px;width:'+(sideBarTdWidth - 40)+'px;overflow:auto');
}