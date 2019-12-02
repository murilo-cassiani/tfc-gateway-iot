<?php
	include("connect.php");
    
    $link=Connection();

    $post = json_decode(file_get_contents('php://input'), true);

 	$latitude=$post["Latitude"];

 	$longitude=$post["Longitude"];

 	$temperature=$post["Temperature"];

 	$humidity=$post["Humidity"];
 
 	$query = "INSERT INTO monitordb.collecteddata (dateTime, latitude, longitude, temperature, humidity) 
					VALUES (current_timestamp,'".$latitude."','".$longitude."','".$temperature."','".$humidity."')"; 
   
	$result = mysql_query($query,$link);

 	mysql_close($link);

 	echo json_encode(array("success"=>$result,"message"=>"Inserido com sucesso"));
     
    //header("Location: index.php");
?>