<?php

 include("connect.php");  
 
 $link=Connection();

 $result=mysql_query("SELECT * FROM collecteddata ORDER BY dateTime DESC",$link);
?>

<html>
  <script type="text/javascript">
    window.onload = function() {
      setTimeout(function(){
        window.location.reload(1);
      }, 90000);
    };
  </script>
  <head>
    <title>ISA CTEEP - Monitoramento</title>
  </head>
<body>
  <a href='http://www.isacteep.com.br/' style='display: block'>
  <img alt='ISA CTEEP' height='200px; ' id='Header1_headerimg' src='index.jpg' style='display: block' width='200px; '/>
</a>
   <h1>Sistema de Monitoramento IoT</h1>
   
   <canvas id="canvasGrafico"></canvas>

   <table border="1" cellspacing="0" cellpadding="0">
  <tr>
   <td align='center' width='160px'><b> Sensor </b></td>
   <td align='center' width='160px'><b> Latitude </b></td>
   <td align='center' width='160px'><b> Longitude </b></td>
   <td align='center' width='160px'><b> Data e Hora </b></td>
   <td align='center' width='160px'><b> Temperatura [°C] </b></td>
   <td align='center' width='160px'><b> Umidade [%] </b></td>
  </tr>

      <?php 
    if($result!==FALSE){
       while($row = mysql_fetch_array($result)) {
          printf("<tr><td align='center'>  %s </td><td>  %s </td><td> %s </td><td> %s </td><td> %s </td><td> %s  </td></tr>", 
             $row['CXEM1'], $row["latitude"], $row["longitude"], $row["dateTime"], $row["temperature"], $row["humidity"]);
       }
       mysql_free_result($result);
       mysql_close();
    }
      ?>

   </table>
</body>
</html>