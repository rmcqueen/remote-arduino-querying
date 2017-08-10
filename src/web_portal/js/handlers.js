/*
* Purpose: enables a user to download the results of their query into a CSV
* formatted file as 'results.csv'
*/
$(document).ready(function () {
    $("#exportButton").click(function () {
        const textAreaData = '[' + $("#resultArea").val() + ']';
        const csv = Papa.unparse(textAreaData);
        const combine = "data:text/csv;charset=utf-8," + csv;
        const encodedUri = encodeURI(combine);
        const download = document.createElement("a");
        download.setAttribute("href", encodedUri);
        download.setAttribute("download", "results.csv");
        document.body.appendChild(download);
        download.click();
    });
});
