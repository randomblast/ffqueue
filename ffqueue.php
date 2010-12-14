<?php

/**
 * Works with the ffqueued script to run batch ffmpeg jobs in the background.
 */
function ffqueue($args)
{
  $cmd = http_build_query($args);
  file_put_contents("/tmp/ffqueue", $cmd, FILE_APPEND);
  return "/tmp/ffqueue-progress/".md5($cmd);
}

ffqueue(array
(
  'i' => $filepath
, 'vcodec' => 'h264'
));
