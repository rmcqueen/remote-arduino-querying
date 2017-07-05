function ajaxCall() {

    var queryBox = $('#publish').val();
    var selectedCheckboxes = "";

    $(":checkbox").each(function () {
        var check = $(this).is(":checked");
        if (check) {
            selectedCheckboxes += $(this).attr('id') + "\n";
        }
    });

    $.ajax({
        url: "",
        type: "get",
        data: {
            secondQuery: queryBox,
            arduino: selectedCheckboxes
        },

        success: function (result) {

            console.log(queryBox);
            console.log(selectedCheckboxes);

            if (queryBox != "") {

                $("#successBox").show();
                $("#successBox").fadeOut(3000);
            }

        }
    });

}