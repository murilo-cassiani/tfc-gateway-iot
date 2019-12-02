<?php
 function Connection(){
  $server="cteepmonitoring-mysqldbserver.mysql.database.azure.com";
  $user="cteep@cteepmonitoring-mysqldbserver";
  $pass="Monitoreo2@19";
  $db="monitordb";
     
  $connection = mysql_connect($server, $user, $pass);
           
  if (!$connection) {
      die('MySQL ERROR: ' . mysql_error());
  }
  
  mysql_select_db($db) or die( 'MySQL ERROR: '. mysql_error() );
  return $connection;
 }
?>