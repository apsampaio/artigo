import path from "path";
import { createWorker } from "tesseract.js";

const worker = createWorker({
  /* logger: (m) => console.log(m), */
});

const recognizePlate = async (name: string, tessdata: string) => {
  await worker.load();
  await worker.loadLanguage(tessdata);
  await worker.initialize(tessdata);
  await worker.setParameters({
    tessedit_char_whitelist: "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ",
  });
  const {
    data: { text },
  } = await worker.recognize(
    path.resolve(__dirname, "../database/images/", name)
  );
  await worker.terminate();
  return text;
};

export default recognizePlate;
