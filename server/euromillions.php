<?php

header('Content-Type: text/plain; charset=utf-8');

$html = @file_get_contents('https://www.euro-millions.com/results');
$exp = '#<tbody>\r?\n<tr class="dateRow">\r?\n<th colspan="5" class="lefty">(\w+) (\d{4})<\/td>\r?\n<\/tr>\r?\n<tr>\r?\n<td class="date"><span>(\w+)<\/span><br>(\d\d?)<sup>\w{2}<\/sup> \1<\/td>\r?\n<td class="centre">\r?\n(\d\d) - (\d\d) - (\d\d) - (\d\d) - (\d\d)<\/td>\r?\n<td class="centre">\r?\n(\d\d) - (\d\d)<\/td>#';

if (preg_match($exp, $html, $matches)) {
	$t = strtotime($matches[4] . ' ' . $matches[1] . ' ' . $matches[2]);
	if ($t) {
		echo date('d/m/Y', $t), '|';
		echo implode(' ', array_slice($matches, 5, 5));
		echo '|';
		echo implode(' ', array_slice($matches, 10, 2));
		echo "\n";
		exit();
	}
}

echo "error\n";
exit();

?>
