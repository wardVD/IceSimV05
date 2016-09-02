#ifndef BELE_H
#define BELE_H

static inline unsigned short be16(const unsigned char *rec) {
  return (rec[0]<<8)|rec[1];
}

static inline unsigned be32(const unsigned char *rec) {
  return (be16(rec)<<16)|be16(rec+2);
}

static inline unsigned long long be48(const unsigned char *rec) {
  return ((unsigned long long)be16(rec)<<32)|be32(rec+2);
}

static inline unsigned long long be64(const unsigned char *rec) {
  return ((unsigned long long)be32(rec)<<32)|be32(rec+4);
}

static inline unsigned short le16(const unsigned char *rec) {
  return (rec[1]<<8)|rec[0];
}

static inline unsigned le32(const unsigned char *rec) {
  return (le16(rec+2)<<16)|le16(rec);
}

static inline unsigned long long le48(const unsigned char *rec) {
  return ((unsigned long long)le16(rec+4)<<32)|le32(rec);
}

static inline unsigned long long le64(const unsigned char *rec) {
  return ((unsigned long long)le32(rec+4)<<32)|le32(rec);
}

#endif
