DROP TABLE IF EXISTS "profile" CASCADE;
DROP TABLE IF EXISTS "group" CASCADE;
DROP TABLE IF EXISTS "profile_group";
DROP TABLE IF EXISTS "session";
DROP TABLE IF EXISTS "tests";
DROP TABLE IF EXISTS "tests_result";
DROP TYPE IF EXISTS role;
DROP TYPE IF EXISTS department;

CREATE TYPE role AS ENUM ('student', 'teacher', 'admin');
CREATE TYPE department AS ENUM ('К', 'ЛТ');

CREATE TABLE "profile"
(
    "id"       SERIAL PRIMARY KEY,
    "login"    VARCHAR(255) NOT NULL,
    "name"     TEXT         NOT NULL,
    "surname"  TEXT         NOT NULL,
    "password" VARCHAR(255) NOT NULL,
    "role"     ROLE         NOT NULL,
    UNIQUE ("login")
);

CREATE TABLE "group"
(
    "id"         SERIAL PRIMARY KEY,
    "department" department NOT NULL,
    "cathedra"   INT        NOT NULL,
    "semester"   INT        NOT NULL,
    UNIQUE ("department", "cathedra", "semester")
);

CREATE TABLE "profile_group"
(
    "profile_id" INT,
    "group_id"   INT,
    PRIMARY KEY ("profile_id", "group_id"),
    FOREIGN KEY ("profile_id") REFERENCES "profile" ("id") ON DELETE CASCADE,
    FOREIGN KEY ("group_id") REFERENCES "group" ("id") ON DELETE CASCADE
);

CREATE TABLE "session"
(
    "uuid"       UUID PRIMARY KEY,
    "profile_id" INT REFERENCES profile (id) ON DELETE CASCADE NOT NULL,
    "user_agent" TEXT                                          NOT NULL,
    "expires"    TIMESTAMP                                     NOT NULL,
    PRIMARY KEY ("uuid", profile_id, user_agent)
);

CREATE TABLE "tests"
(
    "id"          SERIAL PRIMARY KEY,
    "number"      INT  NOT NULL,
    "semester"    INT  NOT NULL,
    "description" TEXT NOT NULL,
    UNIQUE ("number", semester)
);

CREATE TABLE "tests_result"
(
    "id"         SERIAL PRIMARY KEY,
    "test_id"    INT  NOT NULL,
    "profile_id" INT  NOT NULL,
    "result"     TEXT NOT NULL,
    "success"    BOOL NOT NULL,
    FOREIGN KEY ("test_id") REFERENCES "tests" ("id") ON DELETE CASCADE,
    FOREIGN KEY ("profile_id") REFERENCES "profile" ("id") ON DELETE CASCADE
);