<?php

define('MAX_EM_RESULTS', 8);
define('INCLUDE_M1LHAO', true);

header('Content-Type: text/plain; charset=utf-8');

function get_euromillions_results() {
	$results = array();
	$html = @file_get_contents('https://www.euro-millions.com/results');
	$offset = 0; // search offset
	// search for the month header
	while (preg_match('#<tr class="dateRow">\r?\n<th colspan="5" class="lefty">(\w+) (\d{4})</td>\r?\n</tr>#', $html, $matches, PREG_OFFSET_CAPTURE, $offset)) {
		$offset = $matches[0][1] + strlen($matches[0][0]); // move offset
		$month = $matches[1][0];
		$year = $matches[2][0];
		// search for each result
		while (preg_match('#<td class="date"><span>\w+</span><br>(\d\d?)<sup>\w{2}</sup> ' .  preg_quote($month) . '</td>\r?\n<td class="centre">\r?\n\s*<ul class="balls small">\r?\n\s*<li class="new ball">(\d\d?)</li>\r?\n\s*<li class="new ball">(\d\d?)</li>\r?\n\s*<li class="new ball">(\d\d?)</li>\r?\n\s*<li class="new ball">(\d\d?)</li>\r?\n\s*<li class="new ball">(\d\d?)</li>\r?\n\s*</ul>\r?\n</td>\r?\n<td class="centre">\r?\n\s*<ul class="balls small">\r?\n\s*<li class="new lucky-star">(\d\d?)</li>\r?\n\s*<li class="new lucky-star">(\d\d?)</li>\r?\n\s*</ul>\r?\n</td>#', $html, $matches, PREG_OFFSET_CAPTURE, $offset)) {
			$offset = $matches[0][1] + strlen($matches[0][0]); // move offset
			$t = strtotime($matches[1][0] . ' ' . $month . ' ' . $year); // convert date to timestamp
			$results[] = array(
				'type' => 'EM',
				'date' => $t,
				'n' => array_map(function($i) { return str_pad($i[0], 2, '0', STR_PAD_LEFT); }, array_slice($matches, 2, 5)),
				's' => array_map(function($i) { return str_pad($i[0], 2, '0', STR_PAD_LEFT); }, array_slice($matches, 7, 2))
			);
		}
	}
	return $results;
}

function get_m1lhao_results() {
	$results = array();
	$html = @file_get_contents('https://www.euro-millions.com/m1lhao');
	if (preg_match_all('#</span><br>(\w+ \w+ \d{4})</a></td>\r?\n\s*<td style="text-align: center;">\r?\n\s*<div class="millionaire-raffle">(\w+)</div>#', $html, $matches, PREG_SET_ORDER)) {
		foreach ($matches as $match) {
			$results[] = array(
				'type' => 'M1',
				'date' => strtotime($match[1]),
				'value' => $match[2]
			);
		}
	}
	return $results;
}

$all_results = array_slice(get_euromillions_results(), 0, MAX_EM_RESULTS);
if (INCLUDE_M1LHAO) {
	$all_results = array_merge($all_results, get_m1lhao_results());
}

usort($all_results, function($a, $b) {
	if ($a['date'] == $b['date']) {
		return ($a['type'] == 'EM' ? -1 : ($b['type'] == 'EM' ? 1 : 0));
	}
	return $b['date'] - $a['date'];
});

if ($all_results) {
	echo "#\n";
	echo "# EuroMillionsBox server API\n";
	echo "# https://github.com/goncalomb/EuroMillionsBox\n";
	echo "#\n";
	echo "# data scraped from euro-millions.com\n";
	echo "# EM: EuroMillions\n";
	echo "# M1: M1lhão\n";
	echo "#\n";
	foreach ($all_results as $r) {
		echo $r['type'], '|', date('d/m/Y', $r['date']), '|';
		if ($r['type'] == 'EM') {
			echo implode(' ', $r['n']), '|', implode(' ', $r['s']);
		} else if ($r['type'] == 'M1') {
			echo $r['value'];
		}
		echo "\n";
	}
	exit();
}

header('Content-Type: text/plain; charset=utf-8', true, 500);
echo "error\n";
exit();

?>
