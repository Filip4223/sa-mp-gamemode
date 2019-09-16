-- phpMyAdmin SQL Dump
-- version 4.7.4
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Czas generowania: 25 Gru 2018, 13:43
-- Wersja serwera: 10.1.28-MariaDB
-- Wersja PHP: 5.6.32

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Baza danych: `basic`
--

-- --------------------------------------------------------

--
-- Struktura tabeli dla tabeli `samp_players`
--

CREATE TABLE `samp_players` (
  `player_uid` int(11) NOT NULL,
  `player_name` varchar(24) NOT NULL,
  `player_pass` varchar(128) NOT NULL,
  `player_language` tinyint(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Zrzut danych tabeli `samp_players`
--

INSERT INTO `samp_players` (`player_uid`, `player_name`, `player_pass`, `player_language`) VALUES
(3, 'Vincent_Dabrasco', 'cc03e747a6afbbcbf8be7668acfebee5', 1);

--
-- Indeksy dla zrzutów tabel
--

--
-- Indexes for table `samp_players`
--
ALTER TABLE `samp_players`
  ADD PRIMARY KEY (`player_uid`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT dla tabeli `samp_players`
--
ALTER TABLE `samp_players`
  MODIFY `player_uid` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
