import multer from "multer";
import path from "path";
import crypto from "crypto";

const storage = multer.diskStorage({
  destination: path.resolve(__dirname, "../database/images"),

  filename(req, file, cb) {
    const hash = crypto.randomBytes(6).toString("hex");
    const fileName = hash + ".jpeg";
    cb(null, fileName);
  },
});

export const upload = multer({ storage });
