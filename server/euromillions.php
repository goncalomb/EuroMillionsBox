<?php

define('MAX_RESULTS', 8);

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
		while (preg_match('#<td class="date"><span>\w+</span><br>(\d\d?)<sup>\w{2}</sup> ' .  preg_quote($month) . '</td>\r?\n<td class="centre">\r?\n(\d\d) - (\d\d) - (\d\d) - (\d\d) - (\d\d)</td>\r?\n<td class="centre">\r?\n(\d\d) - (\d\d)</td>#', $html, $matches, PREG_OFFSET_CAPTURE, $offset)) {
			$offset = $matches[0][1] + strlen($matches[0][0]); // move offset
			$t = strtotime($matches[1][0] . ' ' . $month . ' ' . $year); // convert date to timestamp
			$results[] = array(
				'date' => date('d/m/Y', $t),
				'n' => array_map(function($i) { return $i[0]; }, array_slice($matches, 2, 5)),
				's' => array_map(function($i) { return $i[0]; }, array_slice($matches, 7, 2))
			);
		}
	}
	return $results;
}

if ($results = get_euromillions_results()) {
	foreach (array_slice($results, 0, MAX_RESULTS) as $r) {
		echo 'EM|', $r['date'], '|', implode(' ', $r['n']), '|', implode(' ', $r['s']), "\n";
	}
	exit();
}

echo "error\n";
exit();

?>
