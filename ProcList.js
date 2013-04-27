//.pragma library

var data = [];

function updateData() {
    data = [
                { label: 'a', value: 60 },
                { label: 'b', value: 40 },
                { label: 'c', value: 40 },
                { label: 'd', value: 7 }
            ];
}

function getData() {
    return data;
}

//(function(){ updateData(); })();

