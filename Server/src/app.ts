import express from "express";
import { upload } from "./config/multer";
import recognizePlate from "./config/tesseract";
import sinespApi from "sinesp-api";

const app = express();

app.use(express.json());

app.post("/placa", upload.single("placa"), async (req, res) => {
  let result;

  try {
    const output = await recognizePlate(req.file.filename, "eng+nummer");
    result = {
      message: "Recognition Without Filter",
      status: "Success",
      imageName: req.file.filename,
      tessdata: "eng+nummer",
      output,
    };
  } catch (err) {
    result = {
      message: "Recognition Without Filter",
      status: "Failed",
      imageName: req.file.filename,
      tessdata: "eng+nummer",
    };
  }

  console.table(result);
  return res.send(result);
});

app.get("/placa", async (req, res) => {
  const { placa } = req.query;
  const info = await sinespApi.search(placa);
  return res.send(info);
});

export default app;
