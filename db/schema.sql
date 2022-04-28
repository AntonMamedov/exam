DROP TABLE IF EXISTS "profile" CASCADE;
DROP TABLE IF EXISTS "group" CASCADE;
DROP TABLE IF EXISTS "profile_group";
DROP TABLE IF EXISTS "session";
DROP TABLE IF EXISTS "tests_result";
DROP TABLE IF EXISTS "tests";
DROP TYPE IF EXISTS role;
DROP TYPE IF EXISTS department;

DROP FUNCTION IF EXISTS create_expires;
DROP FUNCTION IF EXISTS create_date;
DROP FUNCTION IF EXISTS create_group_name;
DROP FUNCTION IF EXISTS check_session;

CREATE TYPE role AS ENUM ('student', 'teacher', 'admin');
CREATE TYPE department AS ENUM ('К', 'ЛТ');

DROP EXTENSION pg_cron;

CREATE EXTENSION IF NOT EXISTS pg_cron;
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

CREATE TABLE "profile"
(
    "id"       SERIAL PRIMARY KEY,
    "login"    VARCHAR(255) NOT NULL,
    "name"     TEXT         NOT NULL,
    "surname"  TEXT         NOT NULL,
    "password" VARCHAR(255) NOT NULL,
    "role"     ROLE[]       NOT NULL,
    UNIQUE ("login")
);

CREATE TABLE "group"
(
    "id"         SERIAL PRIMARY KEY,
    "department" department NOT NULL,
    "cathedra"   INT        NOT NULL,
    "semester"   INT        NOT NULL,
    "group_id"   TEXT       NOT NULL,
    "name"       TEXT       NOT NULL,
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
    "uuid"       UUID PRIMARY KEY default uuid_generate_v4(),
    "profile_id" INT REFERENCES profile (id) ON DELETE CASCADE NOT NULL,
    "user_agent" TEXT                                          NOT NULL,
    "expires"    TIMESTAMP                                     NOT NULL,
    unique ("uuid", profile_id, user_agent)
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
    "test_id"    INT       NOT NULL,
    "profile_id" INT       NOT NULL,
    "result"     TEXT      NOT NULL,
    "success"    BOOL      NOT NULL,
    "date"       TIMESTAMP NOT NULL,
    FOREIGN KEY ("test_id") REFERENCES "tests" ("id") ON DELETE CASCADE,
    FOREIGN KEY ("profile_id") REFERENCES "profile" ("id") ON DELETE CASCADE
);

CREATE FUNCTION create_expires() RETURNS trigger AS
$create_expires$
BEGIN
    NEW.expires := NOW() + INTERVAL '2 months';

    INSERT INTO cron.job (schedule, command, nodeport, database, username)
    VALUES ('15 17 0 0 0', 'DELETE FROM session WHERE id = 1', 5432, 'postgres', 'app');
    RETURN NEW;
END
$create_expires$ LANGUAGE plpgsql;

CREATE TRIGGER create_expires
    BEFORE INSERT
    ON session
    FOR EACH ROW
EXECUTE PROCEDURE create_expires();

CREATE FUNCTION create_date() RETURNS trigger AS
$create_date$
BEGIN
    NEW."date" := NOW();
    RETURN NEW;
END
$create_date$ LANGUAGE plpgsql;

CREATE TRIGGER create_date
    BEFORE INSERT
    ON tests_result
    FOR EACH ROW
EXECUTE PROCEDURE create_date();


CREATE FUNCTION create_group_name() RETURNS trigger AS
$create_group_name$
BEGIN
    NEW.name := NEW.department || cast(NEW.cathedra AS TEXT) || '-' || cast(NEW.semester AS TEXT) || NEW.group_id;
    RETURN NEW;
END
$create_group_name$ LANGUAGE plpgsql;

CREATE TRIGGER create_group_name
    BEFORE INSERT
    ON "group"
    FOR EACH ROW
EXECUTE PROCEDURE create_group_name();


CREATE FUNCTION check_session() RETURNS trigger AS
$check_session$
BEGIN
    if (SELECT EXISTS(SELECT uuid FROM session WHERE profile_id = NEW.profile_id AND user_agent = NEW.user_agent) =
               true) THEN
        DELETE FROM session WHERE profile_id = NEW.profile_id AND user_agent = NEW.user_agent;
    end if;
    RETURN NEW;
END
$check_session$ LANGUAGE plpgsql;

CREATE TRIGGER check_session
    BEFORE INSERT
    ON "session"
    FOR EACH ROW
EXECUTE PROCEDURE check_session();


-- INSERT INTO "group" (department, cathedra, semester, group_id)
-- VALUES ('К', 3, 7, '4Б');
-- SELECT *
-- FROM "group";
--
-- INSERT INTO profile (login, name, surname, password, role)
-- VALUES ('a', 'b', 'c', 'd', '{"student"}');
--
-- INSERT INTO session (profile_id, user_agent)
-- VALUES (1, 'asd');
--
-- SELECT *
-- from session;
--
-- select *
-- from cron.job;


-- SELECT cron.schedule('1 * * * *', $$DELETE FROM session WHERE id = 1$$);