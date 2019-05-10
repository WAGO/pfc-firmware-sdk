var NAVTREE =
[
  [ "OSAL API Manual", "index.html", [
    [ "OSAL LINUX Software Manual", "index.html", null ],
    [ "Modules", "modules.html", [
      [ "Additional Basic types", "group_______t_y_p_e_s__.html", null ],
      [ "System Independent Character Type", "group_______t_c_h_a_r__.html", null ],
      [ "Macros (common)", "group__os__macros__common.html", [
        [ "OS_ARGUMENT_USED", "group___o_s___a_r_g_u_m_e_n_t___u_s_e_d.html", null ],
        [ "OS_MAX_COUNT", "group___o_s___m_a_x___c_o_u_n_t.html", null ],
        [ "OS_ASSERT", "group___o_s___a_s_s_e_r_t.html", null ]
      ] ],
      [ "Swap Macros", "group___o_s___s_w_a_p.html", [
        [ "OS_SWAP_16", "group___o_s___s_w_a_p__16.html", null ],
        [ "OS_SWAP_32", "group___o_s___s_w_a_p__32.html", null ]
      ] ],
      [ "Byte Order Macros", "group___o_s___b_y_t_e___o_r_d_e_r.html", [
        [ "OS_HTON_16", "group___o_s___h_t_o_n__16.html", null ],
        [ "OS_HTON_32", "group___o_s___h_t_o_n__32.html", null ],
        [ "OS_NTOH_16", "group___o_s___n_t_o_h__16.html", null ],
        [ "OS_NTOH_32", "group___o_s___n_t_o_h__32.html", null ]
      ] ],
      [ "Atomic References", "group__os__atomic.html", [
        [ "os_atomic_exchange", "group__os__atomic__exchange.html", null ],
        [ "os_atomic_compare", "group__os__atomic__compare.html", null ],
        [ "os_atomic_increment", "group__os__atomic__increment.html", null ],
        [ "os_atomic_decrement", "group__os__atomic__decrement.html", null ]
      ] ],
      [ "Critical Section References", "group__os__csection.html", [
        [ "os_critical_section_create", "group__os__critical__section__create.html", null ],
        [ "os_critical_section_destroy", "group__os__critical__section__destroy.html", null ],
        [ "os_critical_section_lock", "group__os__critical__section__lock.html", null ],
        [ "os_critical_section_unlock", "group__os__critical__section__unlock.html", null ]
      ] ],
      [ "Memory References", "group__os__memory.html", [
        [ "os_memory_alloc", "group__os__memory__alloc.html", null ],
        [ "os_memory_dealloc", "group__os__memory__dealloc.html", null ]
      ] ],
      [ "Mutual Exclusion References", "group__os__mutex.html", [
        [ "os_mutex_create", "group__os__mutex__create.html", null ],
        [ "os_mutex_destroy", "group__os__mutex__destroy.html", null ],
        [ "os_mutex_lock", "group__os__mutex__lock.html", null ],
        [ "os_mutex_unlock", "group__os__mutex__unlock.html", null ]
      ] ],
      [ "Semaphore References", "group__os__semaphore.html", [
        [ "os_semaphore_create", "group__os__semaphore__create.html", null ],
        [ "os_semaphore_destroy", "group__os__semaphore__destroy.html", null ],
        [ "os_semaphore_post", "group__os__semaphore__post.html", null ],
        [ "os_semaphore_wait", "group__os__semaphore__wait.html", null ]
      ] ],
      [ "Thread References", "group__os__thread.html", [
        [ "os_thread_routine_t", "group__os__thread__routine__t.html", null ],
        [ "os_thread_create", "group__os__thread__create.html", null ],
        [ "os_thread_destroy", "group__os__thread__destroy.html", null ],
        [ "os_thread_exit", "group__os__thread__exit.html", null ]
      ] ],
      [ "Timer References", "group__os__timer.html", [
        [ "os_timeout_routine_t", "group__os__timeout__routine__t.html", null ],
        [ "os_timer_activate_periodical", "group__os__timer__activate__periodical.html", null ],
        [ "os_timer_activate_onetime", "group__os__timer__activate__onetime.html", null ],
        [ "os_timer_deactivate", "group__os__timer__deactivate.html", null ],
        [ "os_timer_stop", "group__os__timer__stop.html", null ],
        [ "os_timer_restart_periodical", "group__os__timer__restart__periodical.html", null ],
        [ "os_timer_restart_onetime", "group__os__timer__restart__onetime.html", null ]
      ] ],
      [ "Time References", "group__os__time.html", [
        [ "os_time_get_time", "group__os__time__get__time.html", null ]
      ] ],
      [ "Structure Packing", "group_______p_a_c_k_i_n_g.html", null ]
    ] ],
    [ "File List", "files.html", [
      [ "os_api.h", null, null ],
      [ "os_compiler.h", null, null ],
      [ "os_specific.h", null, null ]
    ] ]
  ] ]
];

function createIndent(o,domNode,node,level)
{
  if (node.parentNode && node.parentNode.parentNode)
  {
    createIndent(o,domNode,node.parentNode,level+1);
  }
  var imgNode = document.createElement("img");
  if (level==0 && node.childrenData)
  {
    node.plus_img = imgNode;
    node.expandToggle = document.createElement("a");
    node.expandToggle.href = "javascript:void(0)";
    node.expandToggle.onclick = function() 
    {
      if (node.expanded) 
      {
        $(node.getChildrenUL()).slideUp("fast");
        if (node.isLast)
        {
          node.plus_img.src = node.relpath+"ftv2plastnode.png";
        }
        else
        {
          node.plus_img.src = node.relpath+"ftv2pnode.png";
        }
        node.expanded = false;
      } 
      else 
      {
        expandNode(o, node, false);
      }
    }
    node.expandToggle.appendChild(imgNode);
    domNode.appendChild(node.expandToggle);
  }
  else
  {
    domNode.appendChild(imgNode);
  }
  if (level==0)
  {
    if (node.isLast)
    {
      if (node.childrenData)
      {
        imgNode.src = node.relpath+"ftv2plastnode.png";
      }
      else
      {
        imgNode.src = node.relpath+"ftv2lastnode.png";
        domNode.appendChild(imgNode);
      }
    }
    else
    {
      if (node.childrenData)
      {
        imgNode.src = node.relpath+"ftv2pnode.png";
      }
      else
      {
        imgNode.src = node.relpath+"ftv2node.png";
        domNode.appendChild(imgNode);
      }
    }
  }
  else
  {
    if (node.isLast)
    {
      imgNode.src = node.relpath+"ftv2blank.png";
    }
    else
    {
      imgNode.src = node.relpath+"ftv2vertline.png";
    }
  }
  imgNode.border = "0";
}

function newNode(o, po, text, link, childrenData, lastNode)
{
  var node = new Object();
  node.children = Array();
  node.childrenData = childrenData;
  node.depth = po.depth + 1;
  node.relpath = po.relpath;
  node.isLast = lastNode;

  node.li = document.createElement("li");
  po.getChildrenUL().appendChild(node.li);
  node.parentNode = po;

  node.itemDiv = document.createElement("div");
  node.itemDiv.className = "item";

  node.labelSpan = document.createElement("span");
  node.labelSpan.className = "label";

  createIndent(o,node.itemDiv,node,0);
  node.itemDiv.appendChild(node.labelSpan);
  node.li.appendChild(node.itemDiv);

  var a = document.createElement("a");
  node.labelSpan.appendChild(a);
  node.label = document.createTextNode(text);
  a.appendChild(node.label);
  if (link) 
  {
    a.href = node.relpath+link;
  } 
  else 
  {
    if (childrenData != null) 
    {
      a.className = "nolink";
      a.href = "javascript:void(0)";
      a.onclick = node.expandToggle.onclick;
      node.expanded = false;
    }
  }

  node.childrenUL = null;
  node.getChildrenUL = function() 
  {
    if (!node.childrenUL) 
    {
      node.childrenUL = document.createElement("ul");
      node.childrenUL.className = "children_ul";
      node.childrenUL.style.display = "none";
      node.li.appendChild(node.childrenUL);
    }
    return node.childrenUL;
  };

  return node;
}

function showRoot()
{
  var headerHeight = $("#top").height();
  var footerHeight = $("#nav-path").height();
  var windowHeight = $(window).height() - headerHeight - footerHeight;
  navtree.scrollTo('#selected',0,{offset:-windowHeight/2});
}

function expandNode(o, node, imm)
{
  if (node.childrenData && !node.expanded) 
  {
    if (!node.childrenVisited) 
    {
      getNode(o, node);
    }
    if (imm)
    {
      $(node.getChildrenUL()).show();
    } 
    else 
    {
      $(node.getChildrenUL()).slideDown("fast",showRoot);
    }
    if (node.isLast)
    {
      node.plus_img.src = node.relpath+"ftv2mlastnode.png";
    }
    else
    {
      node.plus_img.src = node.relpath+"ftv2mnode.png";
    }
    node.expanded = true;
  }
}

function getNode(o, po)
{
  po.childrenVisited = true;
  var l = po.childrenData.length-1;
  for (var i in po.childrenData) 
  {
    var nodeData = po.childrenData[i];
    po.children[i] = newNode(o, po, nodeData[0], nodeData[1], nodeData[2],
        i==l);
  }
}

function findNavTreePage(url, data)
{
  var nodes = data;
  var result = null;
  for (var i in nodes) 
  {
    var d = nodes[i];
    if (d[1] == url) 
    {
      return new Array(i);
    }
    else if (d[2] != null) // array of children
    {
      result = findNavTreePage(url, d[2]);
      if (result != null) 
      {
        return (new Array(i).concat(result));
      }
    }
  }
  return null;
}

function initNavTree(toroot,relpath)
{
  var o = new Object();
  o.toroot = toroot;
  o.node = new Object();
  o.node.li = document.getElementById("nav-tree-contents");
  o.node.childrenData = NAVTREE;
  o.node.children = new Array();
  o.node.childrenUL = document.createElement("ul");
  o.node.getChildrenUL = function() { return o.node.childrenUL; };
  o.node.li.appendChild(o.node.childrenUL);
  o.node.depth = 0;
  o.node.relpath = relpath;

  getNode(o, o.node);

  o.breadcrumbs = findNavTreePage(toroot, NAVTREE);
  if (o.breadcrumbs == null)
  {
    o.breadcrumbs = findNavTreePage("index.html",NAVTREE);
  }
  if (o.breadcrumbs != null && o.breadcrumbs.length>0)
  {
    var p = o.node;
    for (var i in o.breadcrumbs) 
    {
      var j = o.breadcrumbs[i];
      p = p.children[j];
      expandNode(o,p,true);
    }
    p.itemDiv.className = p.itemDiv.className + " selected";
    p.itemDiv.id = "selected";
    $(window).load(showRoot);
  }
}

