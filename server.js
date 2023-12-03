const { SerialPort } = require("serialport");
const { ReadlineParser } = require("@serialport/parser-readline");
const mongoose = require("mongoose");
const moment = require("moment-timezone");

const IST = "Asia/Kolkata" ;

mongoose
  .connect("mongodb://127.0.0.1:27017/RFID", {})
  .then(() => {
    console.log("db connected");
  })
  .catch((e) => {
    console.log(e);
  });

const DataSchema = mongoose.Schema({
  Value: String,
  timestamp: {
    type: Date,
    default: () => moment().tz(IST).toDate(),
  },
});
const dataModel = mongoose.model("data", DataSchema);

const port = new SerialPort({
  path: "COM4",
  baudRate: 9600,
});

const parser = new ReadlineParser();
port.pipe(parser);

parser.on("data", async (line) => {
  try {
    const newInput = new dataModel({
      Value : line
    });

    
    newInput
      .save()
      .then((savedInput) => {
        console.log("Saved:", savedInput);
      })
      .catch((error) => {
        console.error("Error saving Input:", error);
      });
  } catch (e) {
    console.log(e);
  }
});
