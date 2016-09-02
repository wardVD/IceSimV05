#!/usr/bin/perl

printf("# POSITION DOMID     NAME     MBID\n");
for($string = 21; $string < 101; ++$string)
  {
    for($om = 1; $om <= 60; ++$om)
      {
	$dom = ($string - 21) * 60 + $om;
	printf("%03d-%02d     UP4P%04d  DOM%04d  %012x\n",
	       $string, $om, $dom, $dom, $dom);
      }
  }
